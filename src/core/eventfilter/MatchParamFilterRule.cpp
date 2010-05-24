
#include "MatchParamFilterRule.h"
#include "../../util/StringHelper.h"


namespace dispatch { namespace core { namespace eventfilter {

MatchParamFilterRule::MatchParamFilterRule(GConfigFunctionStatement* node) {
	vector<GConfigScalarVal*> args = node->getArguments();
		if (args.size() != 2) {
			string size = conv_string<size_t>(args.size());
			throw GConfigParseError("match_param requires 2 arguments. Received "+size, node);
		}
		param = args[0]->getStringValue();
		value = args[1]->getStringValue();

}

bool MatchParamFilterRule::match(Event* e) {
	if (!e) {
		return false;
	}
	
	if(!e->hasParameter(param)) {
		return false;
	}
	string v = e->getParameter(param);

	return v == value;
	
}

}}} // end namespace
