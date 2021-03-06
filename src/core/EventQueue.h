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
#include "../config/NodeIdent.h"
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
	* Hash-map for parametrene eventen best�r av
	*/
	map<string, string> parameters;

	config::NodeIdent src_ident;

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
	* Returnerer true dersom eventen er tom, og ikke best�r av noen parametre. Is�fall er den i prinsippet ikke en event
	*/
	bool isEmpty();
	
	
	void setSrcNodeIdent(config::NodeIdent& id);
	
	config::NodeIdent& getSrcNodeIdent();
};

/**
* Interface for event-handlers
*/
class EventHandler {
public:
	/**
	* H�nter en event
	* @todo Definer om denne burde returnere bool
	*/
	virtual void handleEvent(Event* e) = 0;
	
};


/**
* Event-k�en
*/
class EventQueue : public util::BlockingQueue<Event>, public NameTimeTaggingOutputSet {
private:
	
	vector<EventHandler*> handlers;
public:

	EventQueue();

	/**
	* Navnet som vil bli eksponert som navnet p� denne tr�den
	*/
	virtual string getName();

	/**
	* Legg en event i k�en
	*/
	virtual void queue(Event* e);
	
	virtual void queue(Event* e, config::NodeIdent& id);

	/**
	* Start hovedloopen
	*/
	virtual void startListener();
	
	/**
	* Be hovedloopen om � stopp etterhvert
	*/
	virtual void stopListener();
		
	/**
	* Registrer en event-handler p� k�en.
	* Registrerte handlere blir kalt med eventer som kommer ut etter ferden gjennom k�en.
	*/
	virtual void registerHandler(EventHandler* handler);
};

/**
* Event queue which runs all handling in separate threads
*/
class ThreadedEventQueue : public EventQueue {
public:

	ThreadedEventQueue();

}; // end class

}


#endif /* EVENTQUEUE_H_ */
