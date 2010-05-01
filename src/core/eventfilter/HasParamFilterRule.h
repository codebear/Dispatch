#ifndef HAS_PARAM_FILTER_H
#define HAS_PARAM_FILTER_H

#include "EventFilter.h"
#include <string>
namespace dispatch { namespace core { namespace eventfilter {

class HasParamFilterRule : public EventFilterRule {
	string param;
public:
	HasParamFilterRule(string s);
	
	virtual bool match(Event* evnt);

};


}}} // end namespace

#endif
