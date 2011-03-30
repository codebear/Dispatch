#ifndef HAS_PARAM_FILTER_H
#define HAS_PARAM_FILTER_H

#include "EventFilter.h"
#include <string>
namespace dispatch { namespace core { namespace eventfilter {

/**
* Filtrer ut eventer ut fra at et parameter eksisterer
*/
class HasParamFilterRule : public EventFilterRule {
	string param;
public:
	/**
	* Initialiser med konfig-noden som definerer den
	*/
	HasParamFilterRule(GConfigFunctionStatement* node);
	
	/**
	* Prøv regelen mot en event
	*/
	virtual bool match(Event* evnt);

};


}}} // end namespace

#endif
