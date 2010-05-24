#ifndef _EVENT_FILTER_HELPER_H
#define _EVENT_FILTER_HELPER_H 1

#include "EventFilter.h"
#include "../EventQueue.h"
#include "../../config/node_base.h"
#include "../../config/config_nodes.h"

namespace dispatch { namespace core { namespace eventfilter {

/**
* Helper klasse for å lese ut filter fra konfigurasjonsfilen
*/
class EventFilterHelper {
	/**
	* Lag filter-regler uit fra konfig
	*/
	static EventFilterRule* initializeEventFilterRule(GConfigFunctionStatement* node);

public:
	/**
	* Initialiser et komplett filter fra en konfig-node som bør være en filter-blokk
	*/
	static EventFilter* initializeEventFilter(GConfigNode* node);
};



}}} // end namespace

#endif
