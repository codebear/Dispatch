
#include "OriginFilterRule.h"
#include "../../util/StringHelper.h"

namespace dispatch { namespace core { namespace eventfilter {

OriginFilterRule::OriginFilterRule(config::NodeIdent& id) {
	ident = id;
	parent = id.parent();
}
/*	vector<GConfigScalarVal*> args = node->getArguments();
		if (args.size() != 1) {
			string size = conv_string<size_t>(args.size());
			throw GConfigParseError("has_param requires 1 argument. Received "+size, node);
		}
	param = args[0]->getStringValue();

}*/

bool OriginFilterRule::match(Event* e) {
/*	if (!e) {
		return false;
	}
	
	return e->hasParameter(param);
*/
	bool res = parent.contains(e->getSrcNodeIdent());
	cout << "OriginFilterRule: sjekker om eventen " << e->getSrcNodeIdent().getPathStr() << " kan omgås med " << ident.getPathStr() << endl;
	return res;
//return true;
}

}}} // end namespace
