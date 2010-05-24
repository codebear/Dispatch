
#include "EventFilterHelper.h"
#include "HasParamFilterRule.h"
#include "RXParamFilterRule.h"
#include "MatchParamFilterRule.h"
#include "../../config/node_list.h"

using namespace dispatch::config::filter;
namespace dispatch { namespace core { namespace eventfilter {


EventFilter* EventFilterHelper::initializeEventFilter(GConfigNode* node) {
	cout << "Leter etter event-filter " << endl;
	And node_filter(
		new Type(GConfig::FUNCTION), 
		new ContainedIn(new And(
			new Type(GConfig::BLOCK), 
			new Name("filter")
		))
	);
	GConfigNodeList nodes = node->findNodesByFilter(&node_filter);
	GConfigNodeList::iterator node_it;
	EventFilter* ev_filter = new EventFilter();
	int cnt = 0;
	Type parent_block (GConfig::BLOCK);
	
	for(node_it = nodes.begin(); node_it != nodes.end(); node_it++) {
	
		GConfigFunctionStatement* f_stat = dynamic_cast<GConfigFunctionStatement*>(*node_it);
		if (f_stat == NULL) {
			continue;
		}
		
		EventFilterRule* rule = initializeEventFilterRule(f_stat);
		if (rule == NULL) {
			continue;
		}
		
		cnt++;
		ev_filter->addRule(rule);
		f_stat->used(1);
		GConfigNode* block = f_stat->findParentByFilter(&parent_block);
		if (block) {
			block->used(1);
		}
	}
	cout << "Ferdig å leit etter filter... fant " << cnt << " stykk" << endl;
	if (cnt) {
		return ev_filter;
	} else {
		delete ev_filter;
	}
	return NULL;
}


EventFilterRule* EventFilterHelper::initializeEventFilterRule(GConfigFunctionStatement* node) {
	if (node == NULL) {
		cerr << "Mottok en tom filter-regel" << endl;
	}
	cout << "Fant filter med navn: [" << node->getNodeIdent() << "]" << endl;
	vector<GConfigScalarVal*> args = node->getArguments();
	string fname = node->getNodeIdent();
	EventFilterRule* rule = NULL;
	if (fname == "match_param") {
		rule = new MatchParamFilterRule(node);
	} else if (fname == "rx_match_param") {
		rule = new RXParamFilterRule(node);
	} else if (fname == "has_param") {
		rule = new HasParamFilterRule(node);
	}
	return rule;
}


}}} // end namespace

