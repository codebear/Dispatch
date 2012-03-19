
#ifndef _CONFIG_FLEX_DRIVER_H

#define _CONFIG_FLEX_DRIVER_H 1
#ifndef ___FLEX_LEXER_H

#undef yyFlexLexer
#define yyFlexLexer configFlexLexer
#include <FlexLexer.h>
#endif

#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstdlib>
//#include <map>

#include "node_base.h"
#include "config_nodes.h"
#include "NodeIdent.h"
#include "../util/FileStat.h"
//#include "config_y.tab.H"

using namespace std;
//using namespace dispatch::config_parser;
using namespace dispatch;

namespace dispatch {
namespace config {


	class configIncludeFunctionVisitor;
	
	/**
	* Handle for konfigurasjonsfil som er lagt i k� for prosessering
	*/
	class QueuedFile {
	public:
		/**
		* Navnet/sti p� filen
		*/
		string file;
		
		/**
		* Noden denne filen tar utgangspunkt i (rekursiv lasting)
		*/
		GConfigNode* node;

		QueuedFile() {
			node = NULL;
		}

		/**
		* Konstruer med filnavn
		*/
		QueuedFile(string f) {
			file = f;
			node = NULL;
		}

		/**
		* Konstruer med filnavn, og node den skal henge under.
		*/
		QueuedFile(string f, GConfigNode* n) {
			file = f;
			node = n;
		}
	};


	/**
	* Lex-scanner for konfigurasjonsfiler, arver fra scanneren generert av flex, s� denne inneholder bare noen sm�tteri i tillegg.
	*/
	class configScanner : public configFlexLexer {
	
	protected:
		int line;
		int col;
	public:
		/**
		* Streamen det skal leses fra, og hvor logging skal g�
		*/
		configScanner(std::istream* in, std::ostream* out);
		virtual ~configScanner();

		/**
		* Hent ut neste token fra streamen
		* @todo f� lexeren til � h�ndtere posisjon i fila rett
		*/
		config_parser::token_type lex(config_parser::semantic_type* s, config_parser::location_type* loc);
		
		
		void handlePosition(location& loc, char* buf, unsigned int len);
	};

	/**
	* Hoved-driver for konfigurasjonsfil-parseren
	*/
	class parseDriver : public GConfigNodeLookup {
	
		configScanner* _scanner;
		ifstream* _conf_s;
		configScanner* scanner();
		queue<QueuedFile> files;
		string basedir;

		/**
		* Liste over block-listene som er funnet i filene
		*/
		vector<GConfigBlockList*> blocklists;
	public:
		/**
		* Initialiser driver med et filnavn
		*/
		parseDriver(const string& filename);
		
		/**
		* Initialiser driver med filnavn og en basis-katalog
		*/
		parseDriver(const string& filename, const string &basedir);
		
		
		~parseDriver();
		
		/**
		* Legg til en blokkliste (kalles fra parser-koden)
		*/
		void addBlockList(GConfigBlockList* ptr);
		
		/**
		* Dump til stdout info om hva man har, debug-funksjon
		*/
		void dump();
		
		/**
		* Lese ut neste parse-token
		*/
		config_parser::token_type lex(config_parser::semantic_type* s, location* loc);
		
		/**
		* Error underveis i parsingen
		*/
		void error(const location& loc, const string& msg);
		
		/**
		* Kj�r parsing
		*/
		int parse();
		
		/**
		* Kj�r visitor
		*/
		void runVisitor(GConfigNodeVisitor* visitor);
		
		/**
		* Finn noder med et gitt navn, og eventuelt type
		*/
		GConfigNodeList findNodesByName(const string& name, int type = 0);
		
		/**
		* Finn noder ved hjelp av et filter
		*/
		GConfigNodeList findNodesByFilter(filter::NodeFilter* f);
		
		/**
		* Sjekk etter ubrukte noder etter ferdig config-bearbeiding
		*/
		void postCheckConfig();
		
		/**
		* Nullstill scanner, kj�res automatisk f�r hver nye fil blir skannet
		*/
		void resetScanner();
		
		/**
		* Fjerner alt av config, kaller autoreleasepool bl.a.
		*/
		void resetConfigTree();
		
		/**
		* Legg fil i k� for parsing
		*/
		void queueFileForParsing(string file, GConfigNode*);
		
		/**
		* Navn p� fila som er aktiv n�
		*/
		string getFileName();
		
		/**
		* Peker paa fila som parses naa. Er gyldig saa lenge den filen parses
		*/ 
		string* getFileNamePtr();

		string getBaseDir();

		/**
		* Hent ut noden som er aktiv n�
		*/
		GConfigNode* getContainingNode();
	};

	/**
	* Visitor som brukes for � h�ndtere include("conf.fil") og include.d("conf.dir") deklarasjoner i konfigurasjonsfila
	* @todo f� denne til � markere konfig-noder med rett feilmelding dersom fil eller katalog ikke finnes eller ikke lar seg parse
	*/
	class configIncludeFunctionVisitor : public GConfigNodeVisitor {
		parseDriver* driver;
	public:
		/**
		* Initialiser en function-visitor med parse-driveren som trenger den
		*/
		configIncludeFunctionVisitor(parseDriver* dri) {
			driver = dri;
		}

		/**
		* Node som bes�kes
		*/
		virtual void node(GConfigNode* n) {
			if (!n) {
//				printf("Fikk ingenting\n");
				return;
			}
//			printf("Bes�ker node av type %d @ %08x\n", t, n);
			if (n->getTypeId() == GConfig::FUNCTION) {
				GConfigFunctionStatement* func = dynamic_cast<GConfigFunctionStatement*>(n);
				if (!func) {
					return;
				}
				GConfigIdentifier* fname = func->getFunctionName();
				/**
				* @todo F� den her tell � slutt � bruk pod, og heller bruk string
				*/
				char* f = fname->c_str();
				if (f == NULL) {
					return;
				}
//				printf("FUNCTION: %s\n", f);
				if (strcmp(f, "include") == 0) {
					if (!func->used()) {
						func->used(1, 1);
						doInclude(func);
					}
					std::free(f);
					return;
				}
				if (strcmp(f, "include.d") == 0) {
					if (!func->used()) {
						func->used(1, 1);
						doIncludeDir(func);
					}
					std::free(f);
					return;
				}
//				printf("FUNCTION: %s\n", f);
				std::free(f);
			}
			n->visitChildren(this);
		}


		/**
		* Kj�r inkludering av katalog
		*/
		bool doIncludeDir(GConfigFunctionStatement* f) {
			vector<GConfigScalarVal*> args = f->getArguments();
			vector<string> filer = vector<string>();
			bool success = true;
			for(uint i = 0; i < args.size(); i++) {
				GConfigScalarVal* v = args[i];
				if (!v) {
					continue;
				}
				string dir = v->getStringValue();
				if(!dir.length()) {
					continue;
				}
				if (dir[0] != '/') {
					dir = driver->getBaseDir() + "/"+dir;
				}
				cout << "Argument (dir): " << dir << endl;
				if (int err_code = getdir(dir, filer, true)) {
					string err(strerror(err_code));
					v->error(err, err_code);
					success = false;
				}
			}

			for(uint i = 0; i < filer.size(); i ++) {
				string fil = filer[i];
				driver->queueFileForParsing(fil, f->getParentNode());
			}
			return success;
		}

		/**
		* Traverser katalog
		*/
		int getdir (string dir, vector<string> &filer, bool add_dir = false, bool with_dot_files = false) {
		    DIR *dp;
		    struct dirent *dirp;
		    if((dp  = opendir(dir.c_str())) == NULL) {
//		    	dbg(3) << "Error(" << errno << ") opening " << dir << endl;
		        return errno;
		    }

		    while ((dirp = readdir(dp)) != NULL) {
				string entry = string(dirp->d_name);
				if (!with_dot_files && (entry[0] == '.')) {
					continue;
				}
		        filer.push_back(add_dir ? (dir + "/" + entry) : entry);
		    }
		    closedir(dp);
		    return 0;
		}
		

		/**
		* Kj�r inkludering av enkeltfil
		*/
		bool doInclude(GConfigFunctionStatement* f) {
			vector<GConfigScalarVal*> args = f->getArguments();
			for(uint i = 0; i < args.size(); i++) {
				GConfigScalarVal* v = args[i];
				if (!v) {
					continue;
				}
				string file = v->getStringValue();
				util::FileStat stat(file);
				if (stat.isFile()) {
					driver->queueFileForParsing(file, f->getParentNode());
					//dbg(1) << "Inkluderer: " << file << endl;
					v->used(1);
				} else {
					stringstream ss;
					ss << "File " << file << " not found" << endl;
					v->error(ss.str());
				}
			}
			return true;
		}
	};

	/**
	* Debug-visitor som bare brukes for � f� skrevet parset konfigtre til en str�m
	*/
	class dumpTreeVisitor : public GConfigNodeVisitor  {
		/**
		* Node bes�kes
		*/
		virtual void node(GConfigNode* node) {
			string path = "";
			int node_type = node->getTypeId();
			GConfigNode* c_node = NULL;

			switch(node_type) {
			case GConfig::BLOCK_HEADER:
				return;
			case GConfig::STATEMENT_LIST:
				goto children;
			}

			cout << "Node[" << GConfig::getTypeName(node->getTypeId()) << "]: ";
			c_node = node;
			do {
				NodeIdent c_ident = c_node->getFullNodeIdent();
				path = c_ident.getPathStr();
			} while(0);
			cout << path << endl;
		children:
			switch(node_type) {
			case GConfig::VARIABLE:
			case GConfig::FUNCTION:
				return;
			default:
				cout << "  -- Diving into " << GConfig::getTypeName(node_type) << endl;
				node->visitChildren(this);
			}
		}
	};
	
	/**
	 * Check for config-nodes not yet consumed by any modules
	 * Brukes for � finne noder som ingen har tatt ansvar for.
	 * Disse er per deff � regne som konfigurasjonsfilfeil.
	 * Den ser ogs� etter noder som er spesifikt merket som feil
	 */
	class PostCheckConfigVisitor : public GConfigNodeVisitor {
		int error_count;
		int unused_count;
		int node_count;
	public:
		PostCheckConfigVisitor() :
			error_count(0),
			unused_count(0),
			node_count(0)
		{
		}

		/**
		* Node bes�kes
		*/
		virtual void node(GConfigNode* node) {
			node_count++;
			GConfigNode* parent = node->getParentNode();
			if (!node->used()) {
				//if (0) { //parent && !parent->used()) {
					/**
					* If parent isnt used either, we don't need to notify anyone about this,
					* as the parent will trigger a notification. It's easier
					* to understand a notification for the outermost node that is unused.
					*/
				//	++unused_count;
				//} //else {
				
				switch(node->getTypeId()) {
					case GConfig::BLOCK_LIST:
					case GConfig::STATEMENT_LIST:
						/**
						 * These nodes are regularily not used by anyone, so we don't regard these as errors
						 */
//						break;
					default:
						if (parent && !parent->used()) {
							++unused_count;
							break;
						}
						
						cerr << "\nNode: " << node->getClassName() << " - " << node->getNodeName()<< "[" << node->getTypeId() << "] Ident: " <<
							node->getFullNodeIdent().getPathStr() << " location: " << node->getLocation() << " is unused in config" << endl;
						if (parent && !parent->used()) {
							cerr << "\tHas parent which is unused" << endl;
						}
						cerr << "\t" << node->usedStr() << endl;
						cerr << "==== " << node->getClassName() << " ====" << endl;
						cerr << node->getRawContent() << endl;
						cerr << "========" << endl;
						++unused_count;
					}
				//}
			}
			node->visitChildren(this);
		}

		/**
		* Antall noder sjekket
		*/
		int getNodeCount() {
			return node_count;
		}
		
		/**
		*
		*/
		int getUnusedCount() {
			return unused_count;
		}
		
		/**
		* Antall "feil" funnet
		*/
		int getErrorCount() {
			return error_count;
		}
	};

}}
#endif
