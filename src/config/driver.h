
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
//#include "config_y.tab.H"

using namespace std;
//using namespace dispatch::config_parser;
using namespace dispatch;

namespace dispatch {
namespace config {


	class configIncludeFunctionVisitor;
	
	/**
	* Handle for konfigurasjonsfil som er lagt i kø for prosessering
	*/
	class QueuedFile {
	public:
		/**
		* Navnet/sti på filen
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
	* Lex-scanner for konfigurasjonsfiler, arver fra scanneren generert av flex, så denne inneholder bare noen småtteri i tillegg.
	*/
	class configScanner : public configFlexLexer {
	public:
		/**
		* Streamen det skal leses fra, og hvor logging skal gå
		*/
		configScanner(std::istream* in, std::ostream* out);
		virtual ~configScanner();

		/**
		* Hent ut neste token fra streamen
		* @todo få lexeren til å håndtere posisjon i fila rett
		*/
		config_parser::token_type lex(config_parser::semantic_type* s, config_parser::location_type* loc);
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
		* Kjør parsing
		*/
		int parse();
		
		/**
		* Kjør visitor
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
		* Nullstill scanner, kjøres automatisk før hver nye fil blir skannet
		*/
		void resetScanner();
		
		/**
		* Fjerner alt av config, kaller autoreleasepool bl.a.
		*/
		void resetConfigTree();
		
		/**
		* Legg fil i kø for parsing
		*/
		void queueFileForParsing(string file, GConfigNode*);
		
		/**
		* Navn på fila som er aktiv nå
		*/
		string getFileName() {
			if (!files.empty()) {
				return files.front().file;
			}
			return string();
		}
		
		string getBaseDir() {
			return basedir;
		}

		/**
		* Hent ut noden som er aktiv nå
		*/
		GConfigNode* getContainingNode() {
			if (!files.empty()) {
				return files.front().node;
			}
			return NULL;
		}
	};

	/**
	* Visitor som brukes for å håndtere include("conf.fil") og include.d("conf.dir") deklarasjoner i konfigurasjonsfila
	* @todo få denne til å markere konfig-noder med rett feilmelding dersom fil eller katalog ikke finnes eller ikke lar seg parse
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
		* Node som besøkes
		*/
		virtual void node(GConfigNode* n) {
			if (!n) {
//				printf("Fikk ingenting\n");
				return;
			}
//			printf("Besøker node av type %d @ %08x\n", t, n);
			if (n->getTypeId() == GConfig::FUNCTION) {
				GConfigFunctionStatement* func = dynamic_cast<GConfigFunctionStatement*>(n);
				if (!func) {
					return;
				}
				GConfigIdentifier* fname = func->getFunctionName();
				/**
				* @todo Få den her tell å slutt å bruk pod, og heller bruk string
				*/
				char* f = fname->c_str();
				if (f == NULL) {
					return;
				}
//				printf("FUNCTION: %s\n", f);
				if (strcmp(f, "include") == 0) {
					if (!func->used()) {
						func->used(1);
						doInclude(func);
					}
					std::free(f);
					return;
				}
				if (strcmp(f, "include.d") == 0) {
					if (!func->used()) {
						func->used(1);
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
		* Kjør inkludering av katalog
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
				if (getdir(dir, filer, true)) {
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
		        cout << "Error(" << errno << ") opening " << dir << endl;
		        return errno;
		    }

		    while ((dirp = readdir(dp)) != NULL) {
				string entry = string(dirp->d_name);
				if (!with_dot_files && (entry[0] == '.')) {
					continue;
				}
		        filer.push_back(add_dir ? (dir+"/"+entry) : entry);
		    }
		    closedir(dp);
		    return 0;
		}


		/**
		* Kjør inkludering av enkeltfil
		*/
		bool doInclude(GConfigFunctionStatement* f) {
			vector<GConfigScalarVal*> args = f->getArguments();
			for(uint i = 0; i < args.size(); i++) {
				GConfigScalarVal* v = args[i];
				if (!v) continue;
				string file = v->getStringValue();
				driver->queueFileForParsing(file, f->getParentNode());
				cout << "Argument (file): " << file << endl;
			}
			return true;
		}
	};

	/**
	* Debug-visitor som bare brukes for å få skrevet parset konfigtre til en strøm
	*/
	class dumpTreeVisitor : public GConfigNodeVisitor  {
		/**
		* Node besøkes
		*/
		virtual void node(GConfigNode* node) {
			string path = "";
			int cnt = 0;
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
				string c_ident = c_node->getNodeIdent();
				if (c_ident.length()) {
					if (!path.length()) {
						path.append(c_node->getNodeIdent());
					} else {
						path.insert(0, "/");
						path.insert(0, c_node->getNodeIdent());
					}
				}
				if (cnt++>100) {
					cerr << "Breaking loop... this is probably wrong..." << endl;
					break;
				}
			} while( (c_node = c_node->getParentNode()) );
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
	 * Brukes for å finne noder som ingen har tatt ansvar for. Disse er per deff å regne som konfigurasjonsfilfeil.
	 */
	class PostCheckConfigVisitor : public GConfigNodeVisitor {
		int errcnt;
		int node_count;
	public:
		PostCheckConfigVisitor() {
			errcnt = 0;
			node_count = 0;
		}

		/**
		* Node besøkes
		*/
		virtual void node(GConfigNode* node) {
			node_count++;
			if (!node->used()) {
				switch(node->getTypeId()) {
				case GConfig::BLOCK_LIST:
				case GConfig::STATEMENT_LIST:
					/**
					 * These nodes are regularily not used by anyone, so we don't regard these as errors
					 */
					break;
				default:
					cerr << "Node: " << node->getClassName() << " - " << node->getNodeName()<< "[" << node->getTypeId() << "] Ident: " <<
						node->getNodeIdent() << " location: " << node->getLocation() << " is unused in config" << endl;
					errcnt++;
				}
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
		* Antall "feil" funnet
		*/
		int getErrorCount() {
			return errcnt;
		}
	};

}}
#endif
