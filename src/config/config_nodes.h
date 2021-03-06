

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
#include <cstdlib>
#include "location.hh"

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
* @todo bear:05.03.2010 fra denne filen kunne kanskje en god del kode blitt flyttet til cpp-fila, for � f� header-fila litt mer kort og konsis
*/


/**
* Representerer en identifikator-node
* Syntax: n x string separert med punktum(.) ex: mail.maildir.inotify_watcher
*/
class GConfigIdentifier	: public GConfigNode {
	/**
	* Alle stringene identen best�r i
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
		GConfigIdentifier(char* str, location l) : GConfigNode("GConfigIdentifier", l) {
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
		
		virtual string getRawContent(int level) {
			return getStringValue();
		}

		/**
		* Returner node-ident, som er oss selv. :-)
		*/
		virtual NodeIdent getNodeIdent() {
			return getStringValue();
		}
		
		/**
		* Heng p� en til string i identen
		*/
		void appendString(char* str, location l) {
//			std::cout << "Append to identifier: " << str << std::endl;
			tot_len += 1+strlen(str);
			parts.push_back(std::string(str));
			config_loc = config_loc + l;
		}

		/**
		* Hent ut som pod c-string. Uthentere m� selv s�rge for frigj�ring (std::free(ptr));
		*/
		char* c_str() {
			int size = tot_len;
			size += this->parts.size();
			char* buf = (char*)calloc(1, size);
			char* dst = buf;
			for(unsigned int i = 0; i < this->parts.size(); i++) {
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
		* Debug-helper for skriving til str�m
		*/
		virtual std::ostream& operator<<(std::ostream& os) {
			os << "GConfigIdentifier[";
			for(unsigned int i = 0; i < this->parts.size(); i++) {
				if (i) {
					os << ".";
				}
				os << this->parts[i].c_str();
			}
			os << "]";
			return os;
		} // operator<<


		/**
		* Visitor bes�ker denne noden
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
		* Skalare verdier kan v�re enten long, double, char eller identifier
		*/
		union {
			long lval;
			double dval;
			char* cval;
			GConfigIdentifier* ival;
		} scalar;

		/**
		* Hold styr p� hva vi har
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
				default:
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
		GConfigScalarVal(long lval, location l) : GConfigNode("GConfigScalarVal", l) {
			type = LONG;
			scalar.lval = lval;
		};
		
		/**
		* Initialiser med double
		*/
		GConfigScalarVal(double dval, location l) : GConfigNode("GConfigScalarVal", l) {
			type = DOUBLE;
			scalar.dval = dval;
		};
		
		/**
		* Initialiser med identifier
		*/
		GConfigScalarVal(GConfigIdentifier* x) : GConfigNode("GConfigScalarVal", x->getLocation()) {
			type = IDENTIFIER;
			scalar.ival = x;
			x->setParentNode(this);
		};
		
		/**
		* Initialiser med char* buffer
		*/ 
		GConfigScalarVal(char* buf, location l) : GConfigNode("GConfigScalarVal", l) {
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
		* Visitor bes�ker denne noden
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
				default:
					break;
			}
		}

		/**
		* Debug-helper for skriving til str�m
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
				default:
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
					string_stream << scalar.cval;
					res = string_stream.str();
					
//					if (scalar.cval) {
//						res = scalar.cval;
//						return std::string(scalar.cval);
//					}
//					res = scalar.cval ? scalar.cval : "";
					break;
				case IDENTIFIER:
					res = scalar.ival->getStringValue();
			}
			return res;
		}

		virtual string getRawContent(int level) {
			stringstream ss;
			switch(type) {
				case LONG:
				case DOUBLE:
				case IDENTIFIER:
					ss << getStringValue();
					break;
				case CHAR:
					ss << '"' << getStringValue() << '"';
					break;
			}
			return ss.str();
		}

		virtual int used(int u = 0, bool recurse = 0) {
			int i = GConfigNode::used(u, recurse);
			if (type == IDENTIFIER) {
				scalar.ival->used(u, recurse);
			}
			
			
			return i;
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
		* Initialiser argumentliste med f�rste argumentet
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
		* Debug-helper for skriving til str�m
		*/
		virtual std::ostream& operator<<(std::ostream& os) {
			os << "ArgumentList {";
			for(unsigned int i = 0; i < arguments.size(); i++) {
				GConfigScalarVal* val = arguments[i];
				if (val) {
					os << i << ": ";
					os << *val;
				} else {
					os << "Entry "<< i << " i vector va tom" << std::endl;
				}
			}
			return os << "}" << std::endl;
			
		}

		/**
		* Returner node-type-id
		*/
		virtual int getTypeId() {
			return GConfig::ARGUMENT_LIST;
		}

		/**
		* Visitor bes�ker denne noden
		*/
		virtual void visit(GConfigNodeVisitor *visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
			for(unsigned int i = 0; i < arguments.size(); i++) {
				arguments[i]->visit(visitor);
			}
		}
		
		/**
		* Hent ut location i konfig-fil nodene kommer fra
		*/
		virtual location getLocation() {
			int size = arguments.size();
			switch(size) {
				case 0:
					return location();
				case 1:
					return arguments[0]->getLocation();
				default:
					return arguments[0]->getLocation()+arguments[size-1]->getLocation();
			}
		}
		
		/**
		* Merk noden som brukt
		*/ 
	virtual int used(int u = 0, bool recurse = 0) {
		int i = GConfigNode::used(u, recurse);
		if (recurse) {
			std::vector<GConfigScalarVal*>::iterator node_it;
			for(node_it = arguments.begin(); node_it != arguments.end(); node_it++) {
				(*node_it)->used(u, recurse);
			}
		}
		return i;
	}
	
	virtual string getRawContent(int level) {
		std::vector<GConfigScalarVal*>::iterator node_it;
		stringstream ss;
		ss << "(\n";
		
		for(node_it = arguments.begin(); node_it != arguments.end(); ++node_it) {
			if (node_it != arguments.begin()) { //!(i++)) 
				ss << ",\n";
			}
			ss << _indent(level+3) << (*node_it)->getRawContent(level+4);
		}
		ss << "\n" << _indent(level) << ")";
		return ss.str();
	}
};


/**
* Enkelt-argument. Mellom klasse som brukes for � ta vare p� enten en scalar-val, eller en argumentliste dersom man bruke rekursive lister
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
	* Lag argument best�ende av underliggende argumentliste (rekursive deklarasjoner)
	*/
	GConfigArgument(GConfigArgumentList* list) : GConfigNode("GConfigArgument-list") {
		this->arg_list = list;
		list->setParentNode(this);
//		this->str_val = NULL;
		this->scalar_val = NULL;
	}

	/**
	* Argument best�ende av enkel scalar-value
	*/
	GConfigArgument(GConfigScalarVal* val) : GConfigNode("GConfigArgument-val") {
//		this->str_val = NULL;
		this->scalar_val = val;
		val->setParentNode(this);
		this->arg_list = NULL;
	}
	
	/**
	* Hent ut location i konfig-fil til noden
	*/
	virtual location getLocation() {
		if (scalar_val) {
			return scalar_val->getLocation();
		} else if (arg_list) {
			return arg_list->getLocation();
		}
		return location();
	}
		
	/**
	* argument best�ende av bare en string.
	* Denne var en konstruktor som tok en string, men koden kompilerer med denne utkommenter,
	* og den er nok ikke lenger i bruk. Renamer med obsolete_ og lar den ligge intill videre.
	* @todo Er denne faktisk i bruk? Fjern dersom ingen motbevis dukker opp
	*/
/*	void obsolete_GConfigArgument(char* str) : GConfigNode("GConfigArgument-str") {
//		this->str_val = strdup(str);
		this->arg_list = NULL;
		this->scalar_val = NULL;
	}
*/
	/**
	* Returner node-type-id
	*/
	virtual int getTypeId() {
		return GConfig::ARGUMENT;
	}
		
	/**
	* Hent ut enkelt-scalar-val dersom den best�r av det
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
	virtual int used(int i = 0, bool recurse = 0) {
		/**
		* Denne sender videre recurse-argumentet.
		* Vi tar ikke hensyn til dette her, da denne
		* noden bør sees på som den "samme" som det
		* denne peker på.
		*/
		int res = GConfigNode::used(i, recurse);
		if (scalar_val) {
			scalar_val->used(i, recurse);	
		}
		if (arg_list) {
			arg_list->used(i, recurse);	
		}
		return res;
	}
	
	virtual string getRawContent(int level) {
		if (scalar_val) {
			return scalar_val->getRawContent(level);
		}
		if (arg_list) {
			return arg_list->getRawContent(level);
		}
		return string();
	}

	/**
	* Debug-helper for skriving til str�m
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
	* Visitor bes�ker denne noden
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
		* Initialiser blokkliste med navnet p� den
		*/
		GConfigBlockHeader(char* navn, location l) : GConfigNode("GConfigBlockHeader", l), _navn(navn) {
			_ident = NULL;
		}
		
		/**
		* Initialiser blokkliste med navn og ident
		*/
		GConfigBlockHeader(char* navn, GConfigIdentifier* ident, location l) : GConfigNode("GConfigBlockHeader", l), _navn(navn) {
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
		virtual NodeIdent getNodeIdent() {
			if (_ident) {
				return NodeIdent(_ident->getStringValue());
			}
			return NodeIdent();
		}
		/**
		* Returnere navnet p� blokken
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
		
		virtual string getRawContent(int level) {
			stringstream ss;
			ss << _navn;
			if (_ident) {
				ss << " [" << _ident->getRawContent(level) << "]";
			}
			return ss.str();
		}

		/**
		* Debug-helper for skriving til str�m
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
		* Visitor bes�ker denne noden
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
		* Debug-helper for skriving til str�m
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
	* Vektor for � ha disse i
	*/
	std::vector<GConfigStatement*> statements;
	public:

		GConfigStatementList() : GConfigNode("GConfigStatementList") {

		}
		
		/**
		* Initialiser lista med det f�rste elementet
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
			for(unsigned int i = 0; i < statements.size(); i++) {
				GConfigStatement* stmt = statements[i];
				if (stmt) {
					delete stmt;
				}
			}
			statements.clear();
		}

		/**
		* Debug-helper for skriving til str�m
		*/
		virtual ostream& operator<<(ostream& os) {
			os << "GConfigStatementList {" << std::endl;
			for(unsigned int i = 0; i < statements.size(); i++) {
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
		* Visitor bes�ker denne noden
		*/
		virtual void visit(GConfigNodeVisitor* visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
	//		printf("Visitor i lista\n");
			for(unsigned int i = 0; i < statements.size(); i++) {
	//			printf("Element %d\n", i);
				statements[i]->visit(visitor);
			}
		}
		
		virtual string getRawContent(int level) {
			stringstream ss;
			for(unsigned int i = 0; i < statements.size(); ++i) {
				ss << _indent(level) << statements[i]->getRawContent(level+1) << ";\n";
			}
			return ss.str();
		}
		
		/**
		* Hent ut location til noden i konfig-fil
		*/
		virtual location getLocation() {
			int size = statements.size();
			switch(size) {
				case 0:
					return location();
				case 1:
					return statements[0]->getLocation();
				default:
					return statements[0]->getLocation()+statements[size-1]->getLocation();
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
	virtual NodeIdent getNodeIdent() {
		if (_head) {
			return _head->getNodeIdent();
		}
		return NodeIdent();
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
	
	virtual string getRawContent(int level) {
		stringstream ss;
		if (_head) {
			ss << _head->getRawContent(level);
		}
		ss << _indent(level) << "{\n" 
			<< _list->getRawContent(level+1)
			<< _indent(level) << "}";
		return ss.str();
	}


		/**
		* Debug-helper for skriving til str�m
		*/
	virtual std::ostream& operator<<(std::ostream& os);

		/**
		* Visitor bes�ker denne noden
		*/
	virtual void visit(GConfigNodeVisitor* visitor) {
//		printf("Visitor i block\n");
		visitor->node(this);
	}
	
	/**
	* Hent ut location til noden i konfig-fil
	*/
	virtual location getLocation() {
		return _head->getLocation() + _list->getLocation();
	}
	
	virtual int used(int u = 0, int recurse = 0) {
		int i = GConfigNode::used(u, recurse);

		_head->used(u, recurse);

		_list->used(u, recurse);

		return i;
	}

		/**
		* Send visitoren videre til dine barn
		*/
	virtual void visitChildren(GConfigNodeVisitor* visitor) {
		if (_list) {
			_list->visit(visitor);
		}
	}
	
	/**
	* Hent ut Liste over alle statements i blokken
	*/
	GConfigStatementList* getStatementList() {
		return _list;
	}
};

/**
* Liste med blokk-noder
*/
class GConfigBlockList : public GConfigNode {
	/**
	* Vektor for � putt dem i
	*/
	vector<GConfigBlock*> blocks;
	public:

		GConfigBlockList() : GConfigNode("GConfigBlockList") {

		}
		
		/**
		* Initialiser med det f�rste elementet
		*/
		GConfigBlockList(GConfigBlock* b) : GConfigNode("GConfigBlockList") {
			blocks.push_back(b);
			b->setParentNode(this);
		}

		virtual ~GConfigBlockList() {
			for(unsigned int i = 0; i < blocks.size(); i++) {
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
		* Debug-helper for skriving til str�m
		*/
		virtual ostream& operator<<(ostream& os) {
			os << "GConfigBlockList (" << blocks.size() << ") {" << std::endl;
			for(unsigned int i = 0; i < blocks.size(); i++) {
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
		* Visitor bes�ker denne noden
		*/
		virtual void visit(GConfigNodeVisitor* visitor) {
			visitor->node(this);
		}

		/**
		* Send visitoren videre til dine barn
		*/
		virtual void visitChildren(GConfigNodeVisitor* visitor) {
			for(unsigned int i = 0; i < blocks.size(); i++) {
				blocks[i]->visit(visitor);
			}
		}
		
		virtual string getRawContent(int level) {
			stringstream ss;
			for(unsigned int i = 0; i << blocks.size(); ++i) {
				ss << blocks[i]->getRawContent(level+1);
			}
			return ss.str();
		}
		
		virtual int used(int i = 0, bool recurse = 0) {
			if (i) {
				GConfigNode::used(i, recurse);
			}
			return 1;
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
	* Navn p� funksjonen
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
	* Hent ut location til noden i konfig-fil
	*/
	virtual location getLocation() {
		return 
			fname->getLocation() + 
			arguments->getLocation();
	}
	
	virtual string getRawContent(int level) {
		stringstream ss;
		ss << fname->getRawContent(level) 
			<< "" << arguments->getRawContent(level) << "";
		return ss.str();
	}
	
	/**
	* Marker noden som brukt
	*/
	virtual int used(int u = 0, bool recurse = 0) {
		/**
		* Denne havner i et tvilstilfelle hva angår 
		* recursiv merking. Men enn så lenge så merker
		* vi bare argumentlisten dersom vi har recurse slått på
		*/
		int i = GConfigStatement::used(u, recurse);
		fname->used(u, recurse);
		if (recurse) {
			arguments->used(u, recurse);
		}
		return i;
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
	virtual NodeIdent getNodeIdent() {
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
		* Visitor bes�ker denne noden
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
	* Visitor bes�ker denne noden
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
	
	virtual string getRawContent(int level) {
		return identifier->getRawContent(level);
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
		* Initialiser med b�de ident og argument
		*/ 
		GConfigVariableStatement(GConfigIdentifier* ident, GConfigArgument* arg) : GConfigStatement("GConfigVariableStatement") {
			vname = ident;
			value = arg;

			ident->setParentNode(this);
			arg->setParentNode(this);

//			std::cout << "Variable: " << ident << " = " << arg << std::endl;
		}
		
		/**
		* Hent ut location i konfigfil til noden
		*/
		virtual location getLocation() {
			return vname->getLocation()+value->getLocation();
		}

		/**
		* Hent ut ident til variabel
		*/
		virtual NodeIdent getNodeIdent();
		
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
		* Visitor bes�ker denne noden
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
		virtual int used(int i = 0, bool recurse = 0) {
			int res = GConfigStatement::used(i, recurse);
			if (vname) {
				vname->used(i, recurse);
			}
			if (value) {
				value->used(i, recurse);
			}
			return res;
		}
		
		virtual string getRawContent(int level) {
			stringstream ss;
			ss << vname->getRawContent(level) 
				<< " = "
				<< value->getRawContent(level);
			return ss.str();
		}
		
		/**
		* Debug-helper for skriving til str�m
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

