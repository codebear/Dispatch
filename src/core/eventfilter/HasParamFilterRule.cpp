
#include "HasParamFilterRule.h"
#include "../../util/StringHelper.h"

namespace dispatch { namespace core { namespace eventfilter {

HasParamFilterRule::HasParamFilterRule(GConfigFunctionStatement* node) {
	vector<GConfigScalarVal*> args = node->getArguments();
		if (args.size() != 1) {
			string size = conv_string<size_t>(args.size());
			throw GConfigParseError("has_param requires 1 argument. Received "+size, node);
		}
	param = args[0]->getStringValue();

}

bool HasParamFilterRule::match(Event* e) {
	if (!e) {
		return false;
	}
	
	return e->hasParameter(param);
	
}

}}} // end namespace
