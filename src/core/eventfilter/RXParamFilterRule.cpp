
#include "RXParamFilterRule.h"
#include "../../util/StringHelper.h"

using namespace boost;
namespace dispatch { namespace core { namespace eventfilter {

RXParamFilterRule::RXParamFilterRule(GConfigFunctionStatement* node) {
	vector<GConfigScalarVal*> args = node->getArguments();
		if (args.size() != 2) {
			string size = conv_string<size_t>(args.size());
			throw GConfigParseError("rx_match_param requires 2 arguments. Received "+size, node);
		}
	param = args[0]->getStringValue();
	string rx = args[1]->getStringValue();
	expression = new regex(rx);
}

RXParamFilterRule::~RXParamFilterRule() {
	delete expression;
}

bool RXParamFilterRule::match(Event* e) {
	if (!e) {
		return false;
	}
	
	if(!e->hasParameter(param)) {
		return false;
	}
	string val = e->getParameter(param);
	if (regex_match(val.begin(), val.end(), *expression)) {
		return true;
	}
	
	return false;
	
}

}}} // end namespace
