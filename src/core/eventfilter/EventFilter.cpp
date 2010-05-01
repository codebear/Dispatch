
#include "EventFilter.h"
#include "../../config/node_list.h"
#include "HasParamFilterRule.h"
using namespace dispatch::config;
using namespace dispatch::config::filter;
namespace dispatch { namespace core { namespace eventfilter {


EventFilter::EventFilter() {

}

bool EventFilter::addRule(EventFilterRule* rule) {

}

bool EventFilter::match(Event* evnt) {

}


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
		
	} else if (fname == "rx_match_param") {
		
		
	} else if (fname == "has_param") {
		string val = args[0]->getStringValue();
		rule = new HasParamFilterRule(val);
		
		
	}
}


}}} // end namespace

