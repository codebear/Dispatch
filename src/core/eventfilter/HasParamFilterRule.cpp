
#include "HasParamFilterRule.h"


namespace dispatch { namespace core { namespace eventfilter {

HasParamFilterRule::HasParamFilterRule(string p) : param(p) {}

bool HasParamFilterRule::match(Event* e) {
	if (!e) {
		return false;
	}
	
	return e->hasParameter(param);
	
}

}}} // end namespace
