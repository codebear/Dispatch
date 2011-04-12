#include "config.h"
#include "driver.h"
#include <string>
using namespace std;
namespace dispatch {
namespace config {

  const unsigned short int _token_number[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273
  };


			const char*  _table[] = {
			    "\"end of file\"", "error", "$undefined", "T_LPARAN", "T_RPARAN",
				  "T_LBRACE", "T_RBRACE", "T_LBRACKET", "T_RBRACKET", "T_STRING", "T_LONG",
		    		"T_DOUBLE", "T_COMMENT", "T_WHITESPACE", "T_QUOTED_STRING", "T_DOT",
			  	"T_STMT_TERM", "T_ASSIGN", "T_COMMA", "$accept", "start", "block_list",
			  	  "block_start", "block", "identifier", "inner_statement_list",
				  "inner_statement", "variable_assignment", "define_statement",
				    "function_statement", "argument", "argument_list", "scalar_val", 0
			};

			const char* token_lookup(config_parser::token_type tok) {
				for(uint i = 0; i < sizeof(_token_number); i++) {
					if (_token_number[i] == tok) {
						return _table[i];
					}
				}
				return "(UNKNOWN TOKEN)";
			}

	config_parser::token_type parseDriver::lex(config_parser::semantic_type* s, location* loc) {

		config_parser::token_type tok;

	new_token:
		// Read next token
		tok = scanner()->lex(s, loc);
		const char* string_tok = token_lookup(tok);
		std::cout << "Token: " << (string_tok ? string_tok : "(NULL)") << " (";
		if (loc->begin.filename) {
			std::cout << *(loc->begin.filename) << ": ";
		}
		std::cout <<
			loc->begin.line << ":" << loc->begin.column << " - " <<
			loc->end.line << ":" << loc->end.column << ")" << std::endl;

		if (tok == config_parser::token::T_WHITESPACE || tok == config_parser::token::T_COMMENT) {
//			std::cout << "Skipping whitespace" << std::endl;
			goto new_token;
		}
		return tok;
	}
	
	void configScanner::handlePosition(location& loc, char* buf, unsigned int len) {
		loc.begin.line = line;
		loc.begin.column = col;
		for(unsigned int i = 0; i < len; i++) {
			++col;
			if (buf[i] == '\n') {
				++line;
				col = 0;
			}
		}
		loc.end.line = line;
		loc.end.column = col;
	}



	void parseDriver::error(const location& loc, const string& msg) {
		std::cout << "Parse error: " << msg << " in ";
		if (loc.begin.filename) {
			std::cout  << *(loc.begin.filename) << ":";
		}
		std::cout << loc.begin.line <<
			"(l:" << loc.begin.line << ",c:" << loc.begin.column <<
			" <-> l:" << loc.end.line << ",c:" << loc.end.column <<
			")" << endl;
/*		std::printf("Parse error: %s in %s:%d (l:%d,c:%d<->l:%d,c:%d)\n",
			msg.c_str(),
			loc.begin.filename->c_str(),
			loc.begin.line,
			loc.begin.line, loc.begin.column,
			loc.end.line, loc.end.column
		);*/
	}

	parseDriver::parseDriver(const string& filename) : 
		_scanner(NULL), 
		_conf_s(NULL)
	{
		files.push(QueuedFile(filename));
	}

	parseDriver::parseDriver(const string &filename, const string &basename) : 
		_scanner(NULL), 
		_conf_s(NULL) , 
		basedir(basename)
	{
		files.push(QueuedFile(basedir+"/"+filename));
	}

	parseDriver::~parseDriver() {
		resetScanner();
		resetConfigTree();
	}
	
	void parseDriver::resetConfigTree() {
		for(uint i = 0; i < blocklists.size(); i++) {
			GConfigBlockList* block_l = blocklists[i];
			if (block_l) {
				delete block_l;
			}
		}
		blocklists.clear();
		GConfigAutoreleasePool::getInstance()->autorelease();
	}

	void parseDriver::resetScanner() {
		if (_scanner) {
			delete _scanner;
			_scanner = NULL;
		}
		if (_conf_s) {
			delete _conf_s;
			_conf_s = NULL;
		}
	}

	configScanner* parseDriver::scanner() {
		if (!_scanner) {
			string fname = getFileName();
			cout << "Creating new scanner for file " << fname << endl;
			_conf_s = new std::ifstream(fname.c_str());

			_scanner = new configScanner(_conf_s, &std::cout);
		}
		return _scanner;
	}

	void parseDriver::runVisitor(GConfigNodeVisitor* visitor) {
		for(uint i = 0; i < blocklists.size(); i++) {
//			printf("Runs visitor on Elemend %d\n", i);
			blocklists[i]->visit(visitor);
		}
	}

	GConfigNodeList parseDriver::findNodesByName(const string& name, int type) {
		filter::And filter(new filter::Name(name), new filter::Type(type));
		GConfigNodeSearchVisitor vst(&filter);
		runVisitor(&vst);
		return vst.getNodeList();
	}

	GConfigNodeList parseDriver::findNodesByFilter(filter::NodeFilter* f) {
		GConfigNodeSearchVisitor vst(f);
		runVisitor(&vst);
		return vst.getNodeList();
	}

	int parseDriver::parse() {
		try {
			while(!files.empty()) {
				resetScanner();
				std::cout << "Parsing file: " << getFileName() << std::endl;
				config_parser* p = new config_parser(*this);
				int res = p->parse();
				delete p;
				std::cout << "Result: " << res << std::endl;
				files.pop();
				if (res) {
					cerr << "Parse failed (" << getFileName() << ")" << endl;
					return res;
				}
				configIncludeFunctionVisitor includes(this);
				runVisitor(&includes);
			}
			//runVisitor(new dumpTreeVisitor());
		} catch(GConfigParseError ex) {
			cerr << "Parse error (ex: " << ex.getMessage() << ")" << endl;
		}
		return 0;
	}

	void parseDriver::queueFileForParsing(string file, GConfigNode* ptr) {
		files.push(QueuedFile(basedir+"/"+file, ptr));
//		files.
	}

	void parseDriver::addBlockList(GConfigBlockList* block) {
		GConfigNode* node = getContainingNode();
		if (node != NULL) {
			cout << "Adding to inner node" << endl;
			vector<GConfigBlock*> blokker = block->getBlocks();
			for(uint i = 0; i < blokker.size(); i++) {
				if (!node->addChildNode(blokker[i])) {
					throw new GConfigParseError("ParseError, addChildNode did not accept given node");
				}
			}
		} else {
			cout << "Adding to outer node" << endl;
			blocklists.push_back(block);
		}
	}
	void parseDriver::postCheckConfig() {
		PostCheckConfigVisitor vst;
		/*for(int i = 0; i < blocklists.size(); i++) {
			blocklists[i]->used(1);	
		}*/
		runVisitor(&vst);
		int errcnt = vst.getErrorCount();
		cerr << "Fant " << vst.getNodeCount() << " noder i config" << endl;
		if (errcnt) {
			cerr << "Fant " << errcnt << " feil i config" << endl;
		} else {
			cerr << "Config OK!" << endl;	
		}
	}

	void parseDriver::dump() {
		for(uint i = 0; i < blocklists.size(); i++) {
			std::cout << "============= " << i << " =================" << std::endl;
			std::cout << blocklists[i] << std::endl;
		}
	}

}}
