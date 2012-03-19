#ifndef ORIGIN_FILTER_H
#define ORIGIN_FILTER_H

#include "EventFilter.h"
#include <string>
#include "../../config/NodeIdent.h"

namespace dispatch { namespace core { namespace eventfilter {

/**
* Filtrer ut eventer ut fra at et parameter eksisterer
*/
class OriginFilterRule : public EventFilterRule {
//	string param;
	config::NodeIdent ident;
	config::NodeIdent parent;
public:
	/**
	* Initialiser med konfig-noden som definerer den
	*/
	OriginFilterRule(config::NodeIdent& id);
	
	/**
	* Prøv regelen mot en event
	*/
	virtual bool match(Event* evnt);

};


}}} // end namespace

#endif
