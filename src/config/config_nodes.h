

#ifndef _CONFIG_NODES_H

#define _CONFIG_NODES_H 1

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include "node_base.h"
#include "node_list.h"
#include <string.h>
#include <stdlib.h>

#ifdef yylex
#undef yylex
#endif

using namespace std;

namespace dispatch {
namespace config {


class GConfigScalarVal;
class GConfigStatement;
class GConfigArgumentList;
class GConfigArgument;
class GConfigIdentifier;
class GConfigBlock;
class GConfigBlockList;
class GConfigStatementList;

/**
* @todo bear:05.03.2010 fra denne filen kunne kanskje en god del kode blitt flyttet til cpp-fila, for å få header-fila litt mer kort og konsis
*/


/**
* Representerer en identifikator-node
* Syntax: n x string separert med punktum(.) ex: mail.maildir.inotify_watcher
*/
class GConfigIdentifier	: public GConfigNode {
	/**
	* Alle stringene identen består i
	*/
	std::vector<std::string> parts;
	int tot_len;

	public:

		GConfigIdentifier() : GConfigNode("GConfigIdentifier") {
			tot_len = 0;
		}
		
		/**
		* Intialiser med deler av identen
		*/
		GConfigIdentifier(char* str) : GConfigNode("GConfigIdentifier") {
//			std::cout << "New identifier: [" << str << "]" << std::endl;
			parts.push_back(std::string(str));
			tot_len = strlen(str);
		}

		virtual ~GConfigIdentifier() {
			parts.clear();
		}

		/**
		* Returner node-type-id
		*/
		virtual int getTypeId() {
			return GConfig::IDENTIFIER;
		}

		/**
		* Returner node-ident, som er oss selv. :-)
		*/
		virtual string getNodeIdent() {
			return getStringValue();
		}

		/**
		* Heng på en til string i identen
		*/
		void appendString(char* str) {
//			std::cout << "Append to identifier: " << str << std::endl;
			tot_len += 1+strlen(str);
			parts.push_back(std::string(str));
		}

		/**
		* Hent ut som pod c-string. Uthentere må selv sørge for frigjøring (std::free(ptr));
		*/
		char* c_str() {
			int size = tot_len;
			size += this->parts.size();
			char* buf = (char*)calloc(1, size);
			char* dst = buf;
			for(int i = 0; i < this->parts.size(); i++) {
				const char* src = this->parts[i].c_str();
				if (i) {
					*dst++ = '.';
				}
				strcpy(dst, src);
				dst += strlen(src);
			}
			return buf;
		} // c_str

		/**
		* Debug-helper for skriving til strøm
		*/
		virtual std::ostream& operator<<(std::ostream& os) {
			os << "GConfigIdentifier[";
			for(int i = 0; i < this->parts.size(); i++) {
				if (i) {
					os << ".";
				}
				os << this->parts[i].c_str();
			}
			os << "]";
			return os;
		} // operator<<


		/**
		* Visitor besøker denne noden
		*/
		virtual void visit(GConfigNodeVisitor* visitor) {
			visitor->node(this);
		} // visit

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* v) {}

		/**
		* Hent ut string-verdi
		*/
		std::string getStringValue() {
			char* tmp = c_str();
			std::string res = tmp;
			std::free(tmp);
			return res;
		}
//	gc_identifier;

};


/**
* Scalare verdier lest inn fra konfig-fil
*/
class GConfigScalarVal : public GConfigNode {
		/**
		* Skalare verdier kan være enten long, double, char eller identifier
		*/
		union {
			long lval;
			double dval;
			char* cval;
			GConfigIdentifier* ival;
		} scalar;

		/**
		* Hold styr på hva vi har
		*/
		enum _type_t {
			LONG,
			DOUBLE,
			CHAR,
			IDENTIFIER
		} type;
	public:

		virtual ~GConfigScalarVal() {
			switch(type) {
				case CHAR:
					if (scalar.cval) {
						free(scalar.cval);
					}
					break;
				case IDENTIFIER:
					if (scalar.ival){
						delete(scalar.ival);
					}
					break;
			}
		}

		GConfigScalarVal() : GConfigNode("GConfigScalarVal") {
			scalar.ival = NULL;
			scalar.cval = NULL;
		}

		/**
		* Initialiser med long
		*/
		GConfigScalarVal(long lval) : GConfigNode("GConfigScalarVal") {
			type = LONG;
			scalar.lval = lval;
		};
		
		/**
		* Initialiser med double
		*/
		GConfigScalarVal(double dval) : GConfigNode("GConfigScalarVal") {
			type = DOUBLE;
			scalar.dval = dval;
		};
		
		/**
		* Initialiser med identifier
		*/
		GConfigScalarVal(GConfigIdentifier* x) : GConfigNode("GConfigScalarVal") {
			type = IDENTIFIER;
			scalar.ival = x;
			x->setParentNode(this);
		};
		
		/**
		* Initialiser med char* buffer
		*/ 
		GConfigScalarVal(char* buf) : GConfigNode("GConfigScalarVal") {
			type = CHAR;
			scalar.cval = strdup(buf);
		};

		/**
		* Hent ut type-id
		*/ 
		virtual int getTypeId() {
			return GConfig::SCALAR_VAL;
		}

		/**
		* Visitor besøker denne noden
		*/
		virtual void visit(GConfigNodeVisitor* visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
			switch(type) {
				case IDENTIFIER:
					scalar.ival->visit(visitor);
					break;
			}
		}

		/**
		* Debug-helper for skriving til strøm
		*/
		virtual std::ostream& operator<<(std::ostream& out) {

			out << "SCALAR(";
			switch(type) {
				case LONG:
					out << "Long: " << scalar.lval;
					break;
				case DOUBLE:
					out << " Double: " << scalar.dval;
					break;
				case CHAR:
					out << " String: " << (scalar.cval ? scalar.cval : "(NULL)");
					break;
				case IDENTIFIER:
					out << " Identifier: " << *(scalar.ival) << std::endl;
					break;
			}
			out << ")";
			return out;
		}

		/**
		* Hent ut string-verdi av scalar
		*/
		std::string getStringValue() {
			std::string res;
			std::ostringstream string_stream;
			switch(type) {
				case LONG:
					string_stream << scalar.lval;
					res = string_stream.str();
					break;
				case DOUBLE:
					string_stream << scalar.dval;
					res = string_stream.str();
					break;
				case CHAR:
//					printf("BUF: [%s]\n", scalar.cval);
					if (scalar.cval) {
						return std::string(scalar.cval);
					}
//					res = scalar.cval ? scalar.cval : "";
					break;
				case IDENTIFIER:
					res = scalar.ival->getStringValue();
			}
			return res;
		}
};

/**
* List med argumenter, som regel til en variabel-node
*/
class GConfigArgumentList : public GConfigNode {
	/**
	* Vektor med argumenter i lista
	*/
	std::vector<GConfigScalarVal*> arguments;
// gc_argument_list;
	public:
		GConfigArgumentList() : GConfigNode("GConfigArgumentList") {

		}

		/**
		* Initialiser argumentliste med første argumentet
		*/
		GConfigArgumentList(GConfigScalarVal* val) : GConfigNode("GConfigArgumentList") {
			arguments.push_back(val);
			val->setParentNode(this);
		}

		virtual ~GConfigArgumentList();

		/**
		* Legg til argumenter i lista
		*/ 
		void appendArgument(GConfigScalarVal* val) {
			arguments.push_back(val);
			val->setParentNode(this);
		}

		/**
		* Hent ut argumentene i lista
		*/
		std::vector<GConfigScalarVal*> getArguments() {
			return arguments;
		}


		/**
		* Debug-helper for skriving til strøm
		*/
		virtual std::ostream& operator<<(std::ostream& os) {
			os << "ArgumentList {";
			for(int i = 0; i < arguments.size(); i++) {
				GConfigScalarVal* val = arguments[i];
				if (val) {
					os << i << ": ";
					os << *val;
				} else {
					os << "Entry "<< i << " i vector va tom" << std::endl;
				}
			}
			os << "}" << std::endl;
			
		}

		/**
		* Returner node-type-id
		*/
		virtual int getTypeId() {
			return GConfig::ARGUMENT_LIST;
		}

		/**
		* Visitor besøker denne noden
		*/
		virtual void visit(GConfigNodeVisitor *visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
			for(int i = 0; i < arguments.size(); i++) {
				arguments[i]->visit(visitor);
			}
		}
};


/**
* Enkelt-argument. Mellom klasse som brukes for å ta vare på enten en scalar-val, eller en argumentliste dersom man bruke rekursive lister
*/ 
class GConfigArgument : public GConfigNode {
	//char* str_val;
	GConfigArgumentList* arg_list;
	GConfigScalarVal* scalar_val;

public:
	virtual ~GConfigArgument() {
		if (this->arg_list) {
			delete this->arg_list;
			this->arg_list = NULL;
		}
/*		if (this->str_val) {
			free(this->str_val);
			this->str_val = NULL;
		}*/
		if (scalar_val) {
			delete scalar_val;
			scalar_val = NULL;
		}
	}

	GConfigArgument() : GConfigNode("GConfigArgument-empty") {
		arg_list = NULL;
		scalar_val = NULL;
//		str_val = NULL;
	}

	/**
	* Lag argument bestående av underliggende argumentliste (rekursive deklarasjoner)
	*/
	GConfigArgument(GConfigArgumentList* list) : GConfigNode("GConfigArgument-list") {
		this->arg_list = list;
		list->setParentNode(this);
//		this->str_val = NULL;
		this->scalar_val = NULL;
	}

	/**
	* Argument bestående av enkel scalar-value
	*/
	GConfigArgument(GConfigScalarVal* val) : GConfigNode("GConfigArgument-val") {
//		this->str_val = NULL;
		this->scalar_val = val;
		val->setParentNode(this);
		this->arg_list = NULL;
	}
		
	/**
	* argument bestående av bare en string.
	* Denne var en konstruktor som tok en string, men koden kompilerer med denne utkommenter,
	* og den er nok ikke lenger i bruk. Renamer med obsolete_ og lar den ligge intill videre.
	* @todo Er denne faktisk i bruk? Fjern dersom ingen motbevis dukker opp
	*/
	void obsolete_GConfigArgument(char* str)/* : GConfigNode("GConfigArgument-str")*/ {
//		this->str_val = strdup(str);
		this->arg_list = NULL;
		this->scalar_val = NULL;
	}

	/**
	* Returner node-type-id
	*/
	virtual int getTypeId() {
		return GConfig::ARGUMENT;
	}
		
	/**
	* Hent ut enkelt-scalar-val dersom den består av det
	*/
	virtual GConfigScalarVal* getValue() {
		if (scalar_val) {
			return scalar_val;
		}
		return NULL;
	}

	/**
	* Reduser den til en liste av scalar-vals
	*/
	virtual vector<GConfigScalarVal*> getValues() {
		if (scalar_val) {
			vector<GConfigScalarVal*> args;
			args.push_back(scalar_val);
			return args;
		}
		if (arg_list) {
			return arg_list->getArguments();
		}
		return vector<GConfigScalarVal*>();
	}
		
	/**
	* Marker denne noden som tatt i bruk
	*/
	virtual int used(int i) {
		int res = GConfigNode::used(i);
		if (scalar_val) {
			scalar_val->used(i);	
		}
		if (arg_list) {
			arg_list->used(i);	
		}
		return res;
	}

	/**
	* Debug-helper for skriving til strøm
	*/
	virtual std::ostream& operator<<(std::ostream& os) {
		os << "GConfigArgument [";
/*		if (str_val) {
			os << " str: " << str_val;
		}*/
		if (arg_list) {
			os << " arg_liste: " << *arg_list;
		}
		if (scalar_val) {
			os << " scalar_val: " << *scalar_val;
		}
		os << "]";
		return os;
	}

	/**
	* Visitor besøker denne noden
	*/
	void visit(GConfigNodeVisitor* visitor) {
		visitor->node(this);
	}

	/**
	* Send visitoren videre til dine barn
	*/
	void visitChildren(GConfigNodeVisitor* visitor) {
		if (arg_list) {
			arg_list->visit(visitor);
		}
		if (scalar_val) {
			scalar_val->visit(visitor);
		}
	}
};


/**
* Header til en block
*/ 
class GConfigBlockHeader : public GConfigNode {
	/**
	* 
	*/
	string _navn;
	GConfigIdentifier* _ident;
	public:
		GConfigBlockHeader() : GConfigNode("GConfigBlockHeader") {
			_ident = NULL;
		}
		/**
		* Initialiser blokkliste med navnet på den
		*/
		GConfigBlockHeader(char* navn) : GConfigNode("GConfigBlockHeader"), _navn(navn) {
			_ident = NULL;
		}
		
		/**
		* Initialiser blokkliste med navn og ident
		*/
		GConfigBlockHeader(char* navn, GConfigIdentifier* ident) : GConfigNode("GConfigBlockHeader"), _navn(navn) {
			_ident = ident;
			ident->setParentNode(this);
		}

		/**
		* Returner node-type-id
		*/
		virtual int getTypeId() {
			return GConfig::BLOCK_HEADER;
		}

		/**
		* Hent ut node-ident til blokk-headeren (ekvivalent til node-ident for blokka)
		*/
		virtual string getNodeIdent() {
			if (_ident) {
				return _ident->getStringValue();
			}
			return string();
		}
		/**
		* Returnere navnet på blokken
		*/
		virtual string getNodeName() {
			return _navn;
/*			if (_type) {
				return string(_type);
			}
			return string();*/
		}

		virtual ~GConfigBlockHeader() {
/*			if (_type) {
				delete _type;
			}
			if (_ident) {
				delete _ident;
			}*/
		}

		/**
		* Debug-helper for skriving til strøm
		*/
		virtual std::ostream& operator<<(std::ostream& os) {
			os << "GConfigBlockHeader(";
			os << _navn;
			if (_ident) {
				os << ",";
				os << *_ident;
			}
			os << ")";
			return os;
		}

		/**
		* Visitor besøker denne noden
		*/
		virtual void visit(GConfigNodeVisitor* visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
			if (_ident) {
				_ident->visit(visitor);
			}
		}
};

/**
* Basisklasse som alle konkrete statements arver fra
*/
class GConfigStatement : public GConfigNode {
	public:

		/**
		* Initialiser med klassenavn (for debugging)
		*/
		GConfigStatement(const char* classname) : GConfigNode(classname) {

		}

		virtual ~GConfigStatement() {

		}

		/**
		* Debug-helper for skriving til strøm
		*/
		virtual std::ostream& operator<<(std::ostream& os) {
			os << "GConfigStatement";
			return os;
		}

};

/**
* Liste med statement-deklarasjoner
*/
class GConfigStatementList : public GConfigNode {
	/**
	* Vektor for å ha disse i
	*/
	std::vector<GConfigStatement*> statements;
	public:

		GConfigStatementList() : GConfigNode("GConfigStatementList") {

		}
		
		/**
		* Initialiser lista med det første elementet
		*/
		GConfigStatementList(GConfigStatement* stmt) : GConfigNode("GConfigStatementList") {
			statements.push_back(stmt);
			stmt->setParentNode(this);
		}
		
		/**
		* Legg fil flere statements
		*/ 
		void appendStatement(GConfigStatement* stmt) {
			statements.push_back(stmt);
			stmt->setParentNode(this);
		}

		/**
		* Legg til statement som ukjent child node
		*/
		virtual bool addChildNode(GConfigNode* node) {
			GConfigStatement* stmt = dynamic_cast<GConfigStatement*>(node);
			if (stmt != NULL) {
				appendStatement(stmt);
				return true;
			}
			return false;
		}

		/**
		* Hent type-id
		*/
		virtual int getTypeId() {
			return GConfig::STATEMENT_LIST;
		}

		virtual ~GConfigStatementList() {
			for(int i = 0; i < statements.size(); i++) {
				GConfigStatement* stmt = statements[i];
				if (stmt) {
					delete stmt;
				}
			}
			statements.clear();
		}

		/**
		* Debug-helper for skriving til strøm
		*/
		virtual ostream& operator<<(ostream& os) {
			os << "GConfigStatementListï¿½{" << std::endl;
			for(int i = 0; i < statements.size(); i++) {
				GConfigStatement* stmt = statements[i];
				if (stmt) {
					os << *stmt;
				} else {
					os << "Innslag " << i <<" i vektor e tomt..." << std::endl;
				}

			}
			os << "}" << std::endl;
			return os;
		}

		/**
		* Visitor besøker denne noden
		*/
		virtual void visit(GConfigNodeVisitor* visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
	//		printf("Visitor i lista\n");
			for(int i = 0; i < statements.size(); i++) {
	//			printf("Element %d\n", i);
				statements[i]->visit(visitor);
			}
		}
};


/**
* Blokk-node
*/
class GConfigBlock : public GConfigStatement {
	GConfigBlockHeader* _head;
	GConfigStatementList* _list;
public:
	GConfigBlock() : GConfigStatement("GConfigBlock") {
		_head = NULL;
		_list = NULL;
	}

	/**
	* Intialiser blokk med blokkheader og statementlist
	*/
	GConfigBlock(GConfigBlockHeader* head, GConfigStatementList* list) : GConfigStatement("GConfigBlock") {
		_head = head;
		_list = list;
		head->setParentNode(this);
		list->setParentNode(this);
	}

	virtual ~GConfigBlock();

	/**
	* Hent ut node-type-id
	*/  
	virtual int getTypeId() {
		return GConfig::BLOCK;
	}

	/**
	* Hent ut blokk-ident. Kommer fra en eventuelt blokk-header node under oss
	*/
	virtual string getNodeIdent() {
		if (_head) {
			return _head->getNodeIdent();
		}
		return string();
	}

	/**
	* Hent ut node-navn. Kommer fra en eventuell blokk-header under oss
	*/
	virtual string getNodeName() {
		if (_head) {
			return _head->getNodeName();
		}
		return string();
	}


		/**
		* Debug-helper for skriving til strøm
		*/
	virtual std::ostream& operator<<(std::ostream& os);

		/**
		* Visitor besøker denne noden
		*/
	virtual void visit(GConfigNodeVisitor* visitor) {
//		printf("Visitor i block\n");
		visitor->node(this);
	}

		/**
		* Send visitoren videre til dine barn
		*/
	virtual void visitChildren(GConfigNodeVisitor* visitor) {
		if (_list) {
			_list->visit(visitor);
		}
	}
};

/**
* Liste med blokk-noder
*/
class GConfigBlockList : public GConfigNode {
	/**
	* Vektor for å putt dem i
	*/
	vector<GConfigBlock*> blocks;
	public:

		GConfigBlockList() : GConfigNode("GConfigBlockList") {

		}
		
		/**
		* Initialiser med det første elementet
		*/
		GConfigBlockList(GConfigBlock* b) : GConfigNode("GConfigBlockList") {
			blocks.push_back(b);
			b->setParentNode(this);
		}

		virtual ~GConfigBlockList() {
			for(int i = 0; i < blocks.size(); i++) {
				GConfigBlock* bl = blocks[i];
				if (bl) {
					delete bl;
				}
			}
			blocks.clear();
		}

		/**
		* Hent ut alle blokkene i listen (som vektor)
		*/
		vector<GConfigBlock*> getBlocks() {
			return blocks;
		}

		/**
		* Hent ut node-type-id
		*/
		virtual int getTypeId() {
			return GConfig::BLOCK_LIST;
		}
		
		/**
		* Legg til blokk i listen
		*/
		void appendBlock(GConfigBlock* b) {
			blocks.push_back(b);
			b->setParentNode(this);
		}

		/**
		* Debug-helper for skriving til strøm
		*/
		virtual ostream& operator<<(ostream& os) {
			os << "GConfigBlockList (" << blocks.size() << ") {" << std::endl;
			for(int i = 0; i < blocks.size(); i++) {
				GConfigBlock* bptr = blocks[i];
				if (bptr) {
					os << i << ": ";
					os << *bptr;
/*					os << "Block @ " << bptr;
					bptr->operator<<(os);*/
				} else {
					os << "Entry "<< i << " i vector va tom" << std::endl;
				}
			}
			os << "}" << std::endl;
			return os;
		}

		/**
		* Visitor besøker denne noden
		*/
		virtual void visit(GConfigNodeVisitor* visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
			for(int i = 0; i < blocks.size(); i++) {
				blocks[i]->visit(visitor);
			}
		}

};

/**
* Funksjon-"kall"-node
*
* ex:
*	meta_function_with_evil_powers(42, pi, "BAD");
*/
class GConfigFunctionStatement : public GConfigStatement {
	/**
	* Navn på funksjonen
	*/
	GConfigIdentifier* fname;
	/**
	* Argumentliste
	*/
	GConfigArgumentList* arguments;
public:

	GConfigFunctionStatement() : GConfigStatement("GConfigFunctionStatement") {
		fname = NULL;
		arguments = NULL;
	}

	/**
	* funksjonskall initalisert med ident og argument-liste
	*/
	GConfigFunctionStatement(GConfigIdentifier* ident, GConfigArgumentList* args) : GConfigStatement("GConfigFunctionStatement") {
		fname = ident;
		ident->setParentNode(this);
		arguments = args;
		args->setParentNode(this);
	}

	/**
	* Hent ut node-type-iden
	*/
	virtual int getTypeId() {
		return GConfig::FUNCTION;
	}

	virtual ~GConfigFunctionStatement() {
		if (fname) {
			delete fname;
		}
		if (arguments) {
			delete arguments;
		}
	}

	/**
	* Hent ut Node identen for denne noden
	*/
	virtual string getNodeIdent() {
		if (fname) {
			return fname->getNodeIdent();
		}
		return string();
	}

	/**
	* Hent ut funksjonsnavnet
	*/
	virtual GConfigIdentifier* getFunctionName() {
		return fname;
	}

	/**
	* Hent ut argumentene til funksjonskallet
	*/
	virtual vector<GConfigScalarVal*> getArguments() {
		if (arguments) {
			return arguments->getArguments();
		}
		return vector<GConfigScalarVal*>();
	}

		/**
		* Visitor besøker denne noden
		*/
	virtual void visit(GConfigNodeVisitor* visitor) {
		visitor->node(this);
	}


		/**
		* Send visitoren videre til dine barn
		*/
	virtual void visitChildren(GConfigNodeVisitor* visitor) {
		if (fname) {
			fname->visit(visitor);
		}
		if (arguments) {
			arguments->visit(visitor);
		}
	}
};


/**
* Define-statement
*
* Ex:
*      enable_my_foo_flag;
* 
*/ 
class GConfigDefineStatement : public GConfigStatement {

	GConfigIdentifier* identifier;

public:

	GConfigDefineStatement() : GConfigStatement("GConfigDefineStatement") {
		identifier = NULL;
	}

	/**
	* Initialiser med identifier-node
	*/
	GConfigDefineStatement(GConfigIdentifier* ident) : GConfigStatement("GConfigDefineStatement") {
		identifier = ident;
		ident->setParentNode(this);
	}

	virtual ~GConfigDefineStatement() {
		if (identifier) {
			delete identifier;
		}
	}

	/**
	* Node-type-id
	*/
	virtual int getTypeId() {
		return GConfig::DEFINE;
	}

	/**
	* Visitor besøker denne noden
	*/
	virtual void visit(GConfigNodeVisitor* visitor) {
		visitor->node(this);
	}

		/**
		* Send visitoren videre til dine barn
		*/
	virtual void visitChildren(GConfigNodeVisitor* visitor) {
		if (identifier) {
			identifier->visit(visitor);
		}
	}
};

/**
* Variabel statement
* ex:
*	fancy_variabel = [Argument|ArgumentList]
*	evil = 42;
*	bad = "foo";
* 	embrace = ("everyone", 3.14);
*/

class GConfigVariableStatement : public GConfigStatement {
	GConfigIdentifier* vname;
	GConfigArgument* value;
	public:

		GConfigVariableStatement() : GConfigStatement("GConfigVariableStatement") {
			vname = NULL;
			value = NULL;
		}
		/**
		* Initialiser med både ident og argument
		*/ 
		GConfigVariableStatement(GConfigIdentifier* ident, GConfigArgument* arg) : GConfigStatement("GConfigVariableStatement") {
			vname = ident;
			value = arg;

			ident->setParentNode(this);
			arg->setParentNode(this);

//			std::cout << "Variable: " << ident << " = " << arg << std::endl;
		}

		/**
		* Hent ut ident til variabel
		*/
		virtual string getNodeIdent();
		
		/**
		* Hent ut node-type-id
		*/
		virtual int getTypeId() {
			return GConfig::VARIABLE;
		}

		virtual ~GConfigVariableStatement() {
			if (vname) {
				delete vname;
			}
			if (value) {
				delete value;
			}
		}

		/**
		* Hent ut variabelinnhold
		*/
		GConfigScalarVal* getValue() {
			if (value) {
				return value->getValue();
			}
			return NULL;
		}

		/**
		* Hent ut liste dersom flere argumenter
		*/ 
		vector<GConfigScalarVal*> getValues() {
			if (!value) {
				return vector<GConfigScalarVal*>();
			}
			return value->getValues();
		}

		/**
		* Visitor besøker denne noden
		*/
		virtual void visit(GConfigNodeVisitor* visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
			if (vname) {
				vname->visit(visitor);
			}
			if (value) {
				value->visit(visitor);
			}
		}
		
		/**
		* Marker node som brukt
		*/
		virtual int used(int i) {
			int res = GConfigNode::used(i);
			if (vname) {
				vname->used(i);
			}
			if (value) {
				value->used(i);
			}
			return res;
		}
		
		/**
		* Debug-helper for skriving til strøm
		*/
		virtual ostream& operator<<(ostream& os) {
			os << "GConfigVariableStatement (ident: ";
			if(vname) {
			 	os << vname;
			} else {
				os << "NULL";	
			}
			os << "value: ";
			if (value) {
				os << value;
			} else {
				os << "NULL";
			}
			os << ")" << std::endl;
			return os;
		}
};

}}

#endif

