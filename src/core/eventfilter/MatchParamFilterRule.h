#ifndef MATCH_PARAM_FILTER_H
#define MATCH_PARAM_FILTER_H

#include "EventFilter.h"
#include <string>
namespace dispatch { namespace core { namespace eventfilter {

/**
* Filter-regel som matcher en event med en parameter som har en spesifikk verdi
*/
class MatchParamFilterRule : public EventFilterRule {
	string param;
	string value;
public:
	/**
	* Initialiser med konfig-noden som definerer den
	*/
	MatchParamFilterRule(GConfigFunctionStatement* func);
	
	/**
	* Se om regelen matcher en event
	*/
	virtual bool match(Event* evnt);

};


}}} // end namespace

#endif
