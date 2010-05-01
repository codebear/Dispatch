#ifndef _EVENT_FILTER_H
#define _EVENT_FILTER_H

#include "../EventQueue.h"
#include "../../config/node_base.h"
#include "../../config/config_nodes.h"

using namespace dispatch::config;
namespace dispatch { namespace core { namespace eventfilter {


class EventFilterRule {
public:
	virtual bool match(Event* e) = 0;
};

class EventFilter {
public:
	EventFilter();
	
	bool addRule(EventFilterRule* rule);
	
	bool match(Event* e);

};


class EventFilterHelper {
	static EventFilterRule* initializeEventFilterRule(GConfigFunctionStatement* node);

public:
	static EventFilter* initializeEventFilter(GConfigNode* node);
};


}}} // end namespace

#endif
