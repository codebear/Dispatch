/*
 * node_list.h
 *
 *  Created on: May 24, 2009
 *      Author: bear
 */

#ifndef NODE_LIST_H_
#define NODE_LIST_H_

#include <string>
#include <vector>
#include "NodeIdent.h"

using namespace std;

namespace dispatch {
namespace config {


class GConfigNode;
class GConfigNodeList;

namespace filter {
	class NodeFilter;	
}

/**
* Interface for uthenting av undernoder
*/
class GConfigNodeLookup {
	/**
	* Hent ut noder ut fra navn, og eventuelt type
	*/
	virtual GConfigNodeList findNodesByName(const string& name, int type = 0) = 0;
	/**
	* Hent ut noder ut fra filter
	*/
	virtual GConfigNodeList findNodesByFilter(filter::NodeFilter* flt) = 0;
};

/**
* Container-klasse for en liste med konfigurasjons-noder
*/
class GConfigNodeList : public vector<GConfigNode*>, public GConfigNodeLookup {
public:
	/**
	* Hent ut noder ut fra denne lista med navn og eventuelt type
	*/
	GConfigNodeList findNodesByName(const string& name, int type = 0);
	
	/**
	* Hent ut noder fra denne lista ut fra filter
	*/
	GConfigNodeList findNodesByFilter(filter::NodeFilter* flt);
};

/**
* Visitor-implementasjon for traversering av node-treet. Enda litt defekt iforhold til patternet
* @todo rydd denne opp slik at den blir et renere visitor-pattern
* @see http://en.wikipedia.org/wiki/Visitor_pattern
*/
class GConfigNodeVisitor {
	public:
		/**
		* Node som besøkes
		*/
		virtual void node(GConfigNode* node) = 0;
//		virtual bool diveFrom(GConfigNode* node) = 0;
};


/**
* Node-søke-visitor. Sendes inn i node-treet med et filter, og returnere etterpå en node-liste over noder som matcher filteret
*/
class GConfigNodeSearchVisitor : public GConfigNodeVisitor {
	/**
	* Filteret som skal brukes til å lete med
	*/
	filter::NodeFilter* filter;

	/**
	* Hvor langt inn man er
	*/
	int level;

	/**
	* Noder som har truffet
	*/
	GConfigNodeList noder;
public:
	/**
	* Initialiser med filter, og eventuelt nivå
	*/
	GConfigNodeSearchVisitor(filter::NodeFilter* f, int lvl = 0) :
			filter(f), level(lvl) {
		if (level == 0) {
			level--;
		}
	}
//	virtual ~GConfigNodeSearchVisitor();

	void node(GConfigNode* n);

	/**
	* Hent ut utvalgte noder
	*/
	GConfigNodeList getNodeList() {
		return noder;
	}
};


namespace filter {
	/**
	* Basisklasse for et node-filter api. Som kan brukes til å velge ut utvalg av konfigurasjonsfilnoder
	*/
	class NodeFilter {
	public:
		/**
		* Er noden gyldig ihht til filteret?
		*/
		virtual bool accept(GConfigNode* n) = 0;
		virtual ~NodeFilter() {};
	};

	/**
	* Implementerer logisk eller mellom 2 filter. En node er OK, dersom en av underliggende filtrene sier den er OK.
	* Støtter inntill 4 underliggende filter.
	*/
	class Or : public NodeFilter {
		/**
		* Subfilter
		*/
		NodeFilter* one;
		/**
		* Subfilter
		*/
		NodeFilter* two;
		/**
		* Subfilter
		*/
		NodeFilter* three;
		/**
		* Subfilter
		*/
		NodeFilter* four;
	public:
		/**
		* Initialiser med filter. Subfilter blir slettet på destruksjon
		*/
		Or(NodeFilter* l, NodeFilter* r) : one(l), two(r), three(NULL), four(NULL) { }
		
		/**
		* Initialiser med filter. Subfilter blir slettet på destruksjon
		*/
		Or(NodeFilter* l, NodeFilter* r, NodeFilter* f) : one(l), two(r), three(f), four(NULL) {}

		/**
		* Initialiser med filter. Subfilter blir slettet på destruksjon
		*/
		Or(NodeFilter* l, NodeFilter* r, NodeFilter* f, NodeFilter* s) : one(l), two(r), three(f), four(s) {}
	 	virtual ~Or() {
	 		if (one) {
	 			delete one;
	 		}
	 		if (two) {
	 			delete two;
	 		}
	 		if (three) {
	 			delete three;
	 		}
	 		if (four) {
	 			delete four;
	 		}
	 	}
		virtual bool accept(GConfigNode*n);
	};
	
	/**
	* Godtar alle noder
	*/
	class Any : public NodeFilter {
		bool accept(GConfigNode*n) {
			return true; 
		}
	};

	/**
	* Logisk OG. Godtar en node hvis alle underliggende filtre gjør det.
	*/
	class And : public NodeFilter {
		/**
		* Subfilter
		*/
		NodeFilter* one;
		/**
		* Subfilter
		*/
		NodeFilter* two;
		/**
		* Subfilter
		*/
		NodeFilter* three;
		/**
		* Subfilter
		*/
		NodeFilter* four;
	public:
		/**
		* Initialiser med filter. Subfilter blir slettet på destruksjon
		*/
		And(NodeFilter* l, NodeFilter* r) : one(l), two(r), three(NULL), four(NULL) { }
		/**
		* Initialiser med filter. Subfilter blir slettet på destruksjon
		*/
		And(NodeFilter* l, NodeFilter* r, NodeFilter* f) : one(l), two(r), three(f), four(NULL) {}
		/**
		* Initialiser med filter. Subfilter blir slettet på destruksjon
		*/
		And(NodeFilter* l, NodeFilter* r, NodeFilter* f, NodeFilter* s) : one(l), two(r), three(f), four(s) {}
		virtual ~And() {
			if (one) delete one;
			if (two) delete two;
			if (three) delete three;
			if (four) delete four;
		}
		/**
		* Node ok?
		*/
		virtual bool accept(GConfigNode* n);
	};

	/**
	* Node-type. Filtrer på en spesiell type.
	*/
	class Type : public NodeFilter {
		/**
		* Typen som skal sjekkes mot
		*/
		int type;
	public:
		/**
		* Initialiser med type
		*/
		Type(int t) : type(t) { }

		
		virtual bool accept(GConfigNode* n);

	};

	/**
	* Ident. Filtrer på en spesiell ident
	*/
	class Ident : public NodeFilter {
		/**
		* Identen det skal sjekkes mot
		*/
		NodeIdent ident;
		
		/**
		* Skal vi bare sjekke siste del av identen
		*/
		bool simple;
	public:
		/**
		* Ident med bare string, vil sjekke full ident
		*/
		Ident(const string& id) : ident(id), simple(false) { }
		
		/**
		* Ident med string og flag som sier om vi skal sjekke bare siste eller ikke
		*/
		Ident(const string& id, bool s) : ident(id), simple(s) { }

		/**
		* Ident med bare id, vil sjekke full ident
		*/
		Ident(const NodeIdent& id) : ident(id), simple(false) { }
		
		/**
		* Ident med string og flag som sier om vi skal sjekke bare siste eller ikke
		*/
		Ident(const NodeIdent& id, bool s) : ident(id), simple(s) { }

		/**
		* Tilfredstiller angitt node filtret
		*/
		virtual bool accept(GConfigNode* n);
	};

	/**
	* Navn, filtrer på et spesielt navn.
	*/
	class Name : public NodeFilter {
		/**
		* Navnet det skal filtreres mot
		*/
		string name;
	public:
		/**
		* Inintialiser med navn
		*/
		Name(const string& n) : name(n) { }

		/**
		* Er denne noden gyldig?
		*/
		virtual bool accept(GConfigNode* n);
	};
	
	/**
	* Inneholder. Aksepterer en node dersom den noden inneholder en node som er godkjent av det indre filtret.
	*/
	class Contains : public NodeFilter, GConfigNodeVisitor {
		/**
		* Indre filter, som må matche minst et av barna
		*/
		NodeFilter* inner;
		/**
		* Har vi matchet?
		*/
		bool match;
	public:
		/**
		* Inneholder denne noden en node som matcher det angitte filtret. Hvil destruere det indre filtrer på destruksjon. Så pass på.
		*/
		Contains(NodeFilter* filter) : inner(filter) {};
		~Contains() {
			if (inner) {
				delete inner;
			}
		}
		/**
		 *  For the filter API
		 */
		virtual bool accept(GConfigNode* n);
		
		/**
		 *  For the visitor API
		 */
		virtual void node(GConfigNode* node);
	};
	
	/**
	* Inneholdt i. Aksepterer en node dersom denne ligger under en node som blir godkjent av det indre filteret.
	*/
	class ContainedIn : public NodeFilter {
		/**
		* Indre filter, som må matche foreldre
		*/
		NodeFilter* parent_f;
	public:
		/**
		* Instansier med et indre filter. Dersom dette filtret blir destruert, så vil den også delete det indre filtret. Så vær obs på pekerne dine.
		*/
		ContainedIn(NodeFilter* filter) : parent_f(filter) {};
		~ContainedIn() {
			if (parent_f) {
				delete parent_f;
			}
		}
		/**
		* Sjekk om noden matcher filtret
		*/
		virtual bool accept(GConfigNode* n);
	};
	
	/**
	* Verdi, akspterer en node bare dersom den har en gitt verdi
	*/
	class Value : public NodeFilter {
		/**
		* Verdien som skal sjekkes mot
		*/
		string value;
	public:
	
		/**
		* Instansier med verdien som den må ha
		*/	
		Value(const string& v) : value(v) { }
		
		/**
		* Sjekk om angitt node matcher filtret
		*/
		virtual bool accept(GConfigNode* n);	
	};
	

} // end namespace filter

	/**
	* Hjelpeklasse som henter ut string-verdier fra variabel-deklarasjoner i en blokk
	*/
	class StringVariableHelper {
	public:
		/**
		* Hent alle variabel-deklarasjonsverdier som en vector av stringer
		*/
		static vector<string> getStrings(GConfigNode* node, string ident, int mark_used);
		/**
		* Hent ut en string. Dersom strict så kast exception dersom den finner flere.
		* @todo implementer ferdig exception-kasting
		*/
		static string getString(GConfigNode* node, string ident, int mark_used, bool strict);
	};


} // end namespace config
} // end namespace dispatch

#endif /* NODE_LIST_H_ */
