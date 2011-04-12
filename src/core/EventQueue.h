/*
 * EventQueue.h
 *
 *      Author: bear
 */

#ifndef EVENTQUEUE_H_
#define EVENTQUEUE_H_
#include "../util/BlockingQueue.h"
#include "../util/Thread.h"
#include "../util/NameTimeInserter.h"
#include <vector>
#include <map>

using namespace std;
using namespace dispatch::util;

namespace dispatch {


/**
* Kontainer-objekt for selve eventen. Denne blir sendt omkring.
*/
class Event {
	/**
	* Hash-map for parametrene eventen består av
	*/
	map<string, string> parameters;

public:
	
	/**
	* Les ut parameter med navn <b>key</b>
	*/
	string getParameter(string key);
	
	/**
	* Set parameter med navn <b>key</b> til verdi <b>value</b>
	*/
	void setParameter(string key, string value);
	
	/**
	* Check if event has the given parameter. If the parameter
	* exists and has length zero this still returns true.
	*/
	bool hasParameter(string key);
	
	/**
	* Hent ut alle parametre
	*/
	map<string, string> getParameters();
	
	/**
	* Fjern et parameter fra eventen
	*/
	void clearParameter(string key);
	
	/**
	* Returnerer true dersom eventen er tom, og ikke består av noen parametre. Isåfall er den i prinsippet ikke en event
	*/
	bool isEmpty();
	
};

/**
* Interface for event-handlers
*/
class EventHandler {
public:
	/**
	* Hånter en event
	* @todo Definer om denne burde returnere bool
	*/
	virtual void handleEvent(Event* e) = 0;
	
};


/**
* Event-køen
*/
class EventQueue : public util::BlockingQueue<Event>, public NameTimeTaggingOutputSet {
private:
	/**
	* Instans for singleton-patternet
	*/
//	static EventQueue* _instance;
	
	vector<EventHandler*> handlers;
public:

	EventQueue();

	/**
	* Hent ut singleton-instansen av denne
	*/
/*	static EventQueue* instance() {
		if (!_instance) {
			_instance = new EventQueue();
		}
		return _instance;
	}
*/	
	/**
	* Navnet som vil bli eksponert som navnet på denne tråden
	*/
	virtual string getName() {
		return "ThreadedEventQueue";
	}

	/**
	* Legg en event i køen
	*/
	virtual void queue(Event* e) {
		push(e);
	}

	/**
	* Start hovedloopen
	*/
	virtual void startListener();
	
	/**
	* Be hovedloopen om å stopp etterhvert
	*/
	virtual void stopListener();
		
	/**
	* Registrer en event-handler på køen.
	* Registrerte handlere blir kalt med eventer som kommer ut etter ferden gjennom køen.
	*/
	virtual void registerHandler(EventHandler* handler);
};

/**
* Event queue which runs all handling in separate threads
*/
class ThreadedEventQueue : public EventQueue {
private:
	/**
	* Instans for singleton-patternet
	*/
//	static EventQueue* _instance;
	
//	vector<EventHandler*> handlers;
public:

	ThreadedEventQueue();

	/**
	* Hent ut singleton-instansen av denne
	*/
/*	static EventQueue* instance() {
		if (!_instance) {
			_instance = new EventQueue();
		}
		return _instance;
	}*/

/**
* Registrer ny handler
*/
//	virtual void registerHandler(EventHandler* handler);

}; // end class

}


#endif /* EVENTQUEUE_H_ */
