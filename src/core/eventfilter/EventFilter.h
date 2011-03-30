#ifndef _EVENT_FILTER_H
#define _EVENT_FILTER_H

#include "../EventQueue.h"
#include "../../config/node_base.h"
#include "../../config/config_nodes.h"

using namespace dispatch::config;
namespace dispatch { namespace core { namespace eventfilter {


/**
* Abstract basisklasse for enkelt-regel i et event-filter.
*/
class EventFilterRule {
public:
	virtual ~EventFilterRule();
	/**
	* Sjekk en event mot regelen
	*/
	virtual bool match(Event* e) = 0;
};

/**
* Filter for eventer. Består av N filerregler
*/
class EventFilter {
	vector<EventFilterRule*> rules;
public:
	EventFilter();
	virtual ~EventFilter();
	
	/**
	* Legg til regel i filteret
	*/
	bool addRule(EventFilterRule* rule);
	
	/**
	* Sjekk en event mot alle reglene i filteret
	*/
	bool match(Event* e);

};




}}} // end namespace

#endif
