
#include "EventFilter.h"
using namespace dispatch::config;
using namespace dispatch::config::filter;
namespace dispatch { namespace core { namespace eventfilter {

bool EventFilterRule::match(Event* e) {
	cerr << "FILTER MISSING match IMPLEMENTATION" << endl;
	return false;
}

EventFilterRule::~EventFilterRule() {

}

EventFilter::EventFilter() {

}

EventFilter::~EventFilter() {
	vector<EventFilterRule*>::iterator rule_it;
	for(rule_it = rules.begin(); rule_it != rules.end(); rule_it++) {
		if(*rule_it) {
			delete *rule_it;
		}
	}
}

bool EventFilter::addRule(EventFilterRule* rule) {
	rules.push_back(rule);
	return true;
}

bool EventFilter::match(Event* evnt) {
	vector<EventFilterRule*>::iterator rule_it;
	for(rule_it = rules.begin(); rule_it != rules.end(); rule_it++) {
		if (!((*rule_it)->match(evnt))) {
			return false;
		}
	}
	return true;
}




}}} // end namespace

