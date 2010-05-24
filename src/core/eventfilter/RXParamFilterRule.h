#ifndef RX_PARAM_FILTER_RULE_H
#define RX_PARAM_FILTER_RULE_H

#include "EventFilter.h"
#include <boost/regex.hpp>
#include <string>
namespace dispatch { namespace core { namespace eventfilter {

/**
* Filter-regel som matcher en event dersom den har en spesifikk parameter, og dens verdi matcher et regulært uttrykk
*/
class RXParamFilterRule : public EventFilterRule {
	string param;
	boost::regex* expression;
public:
	/**
	* Initialiser med konfigurasjonsnoden som definererden
	*/
	RXParamFilterRule(GConfigFunctionStatement* node);
	virtual ~RXParamFilterRule();
	
	/**
	* Matcher regelen en event
	*/
	virtual bool match(Event* evnt);

};


}}} // end namespace

#endif
