
#include "EventQueue.h"

#ifndef _QUEUE_PROVIDER_H
#define _QUEUE_PROVIDER_H

using namespace dispatch;

namespace dispatch {

class QueueProvider {
public:
	virtual EventQueue* getEventQueue() = 0;
};

}
#endif
