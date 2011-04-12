
#ifndef _NODE_BASE_H
#define _NODE_BASE_H 1

#include <list>
#include <iostream>
#include "node_list.h"
#include "location.hh"
#include "NodeIdent.h"


using namespace std;

namespace dispatch {
namespace config {

class GConfigAutoreleasePool;

/**
* Type-basis-klasse. Brukes igrunn bare til type-ider
*/
class GConfig {
public:
	/**
	* Type-id til Scalar-val-noder
	*/
	static const int SCALAR_VAL = 		 1;
	/**
	* Type-id til argument-list noder
	*/
	static const int ARGUMENT_LIST = 	 2;
	/**
	* Type-id til argument noder
	*/
	static const int ARGUMENT = 		 3;
	/**
	* Type-id til identifier noder
	*/
	static const int IDENTIFIER = 		 4;
	/**
	* Type-id til block-noder
	*/
	static const int BLOCK = 			 5;
	/**
	* type-id til block-liste-noder
	*/
	static const int BLOCK_LIST = 		 6;
	/**
	* Type-id til block-header-noder
	*/
	static const int BLOCK_HEADER = 	 7;
	/**
	* type-id til statement-list noder
	*/
	static const int STATEMENT_LIST = 	 8;
	/**
	* Type-id til define-noder
	*/
	static const int DEFINE = 			 9;
	/**
	* Type-id til variabel-noder
	*/
	static const int VARIABLE = 		10;
	/**
	* Type id til funksjons noder
	*/
	static const int FUNCTION = 		11;
#	define _TYPE_CASE(x) case x: return #x;

	/**
	* Hent ut type-navn ut fra id int
	*/
	static string getTypeName(int type) {
		switch(type) {
			_TYPE_CASE(SCALAR_VAL);
			_TYPE_CASE(ARGUMENT_LIST);
			_TYPE_CASE(ARGUMENT);
			_TYPE_CASE(IDENTIFIER);
			_TYPE_CASE(BLOCK);
			_TYPE_CASE(BLOCK_LIST);
			_TYPE_CASE(BLOCK_HEADER);
			_TYPE_CASE(STATEMENT_LIST);
			_TYPE_CASE(DEFINE);
			_TYPE_CASE(VARIABLE);
			_TYPE_CASE(FUNCTION);
		default:
			return "<unknown>";
		}
	}
};


class GConfigNode;

/**
* Parse error exception
*/
class GConfigParseError {
	string _msg;
	GConfigNode* _node;
public:
	/**
	* Error tekst
	*/
	GConfigParseError(string m) : _msg(m), _node(NULL) {

	}
	
	/**
	* Error tekst, med relatert node
	*/
	GConfigParseError(string m, GConfigNode* n) : _msg(m), _node(n) {
	
	}
	
	/**
	* Hent ut feilmeldingen fra exceptionen
	*/
	string getMessage() {
		return _msg;
	}
	
	/**
	* Relatert node
	*/
	GConfigNode* node() {
		return _node;
	}
};

/**
* Basisklasse for alle konfigurasjonsfilnodene
*/
class GConfigNode : public GConfigNodeLookup {
	int __mark;

protected:
	/**
	* Klassenavnet til sub-klassen. Til info og debug-bruk
	*/
	const char* _classname;
	
	/**
	* Overliggende node i treet
	*/
	GConfigNode* parent;
	
	/**
	* Har denne noden blitt brukt?
	*/
	int _used;
	
	/**
	* Er denne noden merket som feil
	*/
	int _error;
	
	/**
	* Eventuelt feil-melding
	*/
	string _error_msg;
	
	/**
	* Lokasjon i fila
	*/
	location config_loc;
	
public:

	/**
	* Initialiser med subklassenavnet
	*/
	GConfigNode(const char* classname);	
	
	/**
	* Initialiser med klassenavn og location fra kildefil
	*/
	GConfigNode(const char* classname, location loc);
	
	virtual ~GConfigNode();

	/**
	* Set foreldre-noden til denne
	*/
	void setParentNode(GConfigNode* prnt);

	/**
	* Hent ut foreldre-noden
	*/
	GConfigNode* getParentNode();

	/**
	* Hent ut hvilket nivå denne er på
	*/
	int getLevel();

	/**
	* hent ut denne nodens spesifisert ident
	*/
	virtual NodeIdent getNodeIdent();
	
	/**
	* Hent ut denne nodens fulle ident. Dvs denne nodens ident kjedet sammen med ovenstående noders ident.
	*/
	virtual NodeIdent getFullNodeIdent();
	
	/**
	* Hent ut node-navn
	*/
	virtual string getNodeName();
	
	/**
	* @todo forklar autoreleasepool-greia mer uttypende
	*/
	virtual void removeFromAutoReleasePool();

	/**
	* Legg til underliggende node. Denne må overrides dersom det skal bli lov.
	*/
	virtual bool addChildNode(GConfigNode* node) {
		string msg = "Not allowed to add nodes to a ";
		msg += getClassName();
		throw GConfigParseError(msg);
	}

	/**
	* Finn noder ut fra navn, og eventuelt type
	*/
	virtual GConfigNodeList findNodesByName(const string& name, int type = 0);

	/**
	* Finn noder ut fra filter
	*/
	virtual GConfigNodeList findNodesByFilter(filter::NodeFilter* f);

	/**
	* Finn den første foreldren som matcher angitt filter
	*/	
	virtual GConfigNode* findParentByFilter(filter::NodeFilter* f, int level = 0);

	/**
	* Besøk av visitor
	*/
	virtual void visit(GConfigNodeVisitor* visitor) = 0;
	
	/**
	* Send visitor videre til dine barn
	*/
	virtual void visitChildren(GConfigNodeVisitor* visitor) = 0;

	/**
	* Marker noden, brukes av mark'n'sweep greia til autorelease-poolen
	*/
	int _mark(int i) {
		return __mark = i;
	}
	
	/**
	* Hent ut merket
	*/ 
	int _mark() {
		return __mark;
	}


	/**
	* Hent ut node-type-id
	*/
	virtual int getTypeId()=0;

	/**
	* Hjelpemetode for debugging, Skriv noden til en strøm
	*/
	virtual std::ostream& operator<<(std::ostream& os)=0;
	
	/**
	* Marker noden som brukt
	*/
	virtual int used(int used = 0) {
		return _used += used;
	}

	/**
	* For debugging. Hent ut klassenavn
	*/
	virtual string getClassName() {
		return string(_classname);
	}
	
	/**
	* Hent ut location til noden i kildekonfigurasjonsfil
	*/
	virtual location getLocation() {
		return config_loc;
	}

};

/**
* Brukes for å merke noder som tatt i bruk
*/
class MarkingVisitor : public GConfigNodeVisitor {
	int mark;
public:	
	/**
	* Lag marking visitor som merker med spesifisert tall
	*/
	MarkingVisitor(int mark) {
		this->mark = mark;
	}
	
	/**
	* Node har blitt visited
	*/
	void node(GConfigNode* node) {
		node->_mark(this->mark);
		node->visitChildren(this);
	}
};

/**
* Autorelease-pool brukes for å kunne frigjøre alle noder etter at parseren er ferdig å kjøre. Dersom det er syntax-feil i 
* konfigurasjonsfilen kan parseren bli kakket ut av lag, og dermed blir minnefrigjøring et mareritt. Derfor legger vi 
* alle nodene i en autoreleasepool. Deretter kan vi mark-and sweepe de som er gyldige når parseren er ferdig, resten frigjør vi.
* Denne kan i prinsippet også brukes dersom man skal lese inn ny konfigurasjon, og vil bli kvitt den gamle fra minnet.
*/
class GConfigAutoreleasePool {
	static GConfigAutoreleasePool* instance;
	int last_iteration;

	list<GConfigNode*> pool;
	
	void doAutoreleasing(int keeper_mark) {
		list<GConfigNode*>::iterator ptr(pool.begin());
		list<GConfigNode*>::iterator curr;
		int cnt = 0;
		while(ptr != pool.end()) {
			curr = ptr;
			/**
			* Inkrement med en gang. Dersom vi endra, så invalider vi iteratoren hvis den peik på den som vi fjern
			*/
			++ptr;
			
			if ((*curr)->_mark() == keeper_mark) {
				continue;
			}
			/**
			* Remove from pool
			*/
			std::cout << "Autoreleasing node" << std::endl;
			pool.erase(curr);
			cnt++;
			/**
			* Changing the list can invalidate the iterator, so se rewind with a new iterator
			*/
//			ptr = pool.begin();
		}
		std::cerr << "After autoreleasing " << cnt << " nodes there are " << pool.size() << " elements left" << endl;
	}

public:

	/**
	* Hent ut singleton-instans av auto-releasepoolen
	*/
	static GConfigAutoreleasePool* getInstance() {
		if (!instance) {
			instance = new GConfigAutoreleasePool();
		}
		return instance;
	}

	GConfigAutoreleasePool() {
		last_iteration = 0;
	}

	/**
	* Legg til node i pool
	*/
	void add(GConfigNode* ptr) {
		pool.push_back(ptr);
		ptr->_mark(last_iteration);
	}
	
	/**
	* Fjern node fra pool
	*/
	void remove(GConfigNode* ptr) {
		pool.remove(ptr);
	}
	
	/**
	* Kjør autoreleaseing av alle som ikke har merket fra forrige merkerunde
	*/
	void autorelease() {
		this->doAutoreleasing(++last_iteration);
	}

	/**
	* Kjør autoreleaseing med en vector med noder som ikke skal frigjøres. Dette gjøres med en mark sweep på disse først. 
	*/
	void autorelease(vector<GConfigNode*>& keepers) {
		/**
		* Every node should now be marked with last_iteration.
		* Therefore we start with marking all keepers with next iteration
		*/
		vector<GConfigNode*>::iterator iter(keepers.begin());
		MarkingVisitor vst(++last_iteration);
		while(iter != keepers.end()) {
			(*iter)->visit(&vst);
			++iter;
		}
		this->doAutoreleasing(last_iteration);
	}
};


std::ostream& operator<< (std::ostream& os, GConfigNode& ptr);
std::ostream& operator<< (std::ostream& os, GConfigNode* ptr);


}} // end namespace

#endif
