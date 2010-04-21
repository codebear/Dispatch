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
	* Hash-map for parametrene eventen best�r av
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
class Eventqueue : public util::BlockingQueue<Event>, public NameTimeTaggingOutputSet {
private:
	/**
	* Instans for singleton-patternet
	*/
	static Eventqueue* _instance;
	
	vector<EventHandler*> handlers;
public:

	Eventqueue();

	/**
	* Hent ut singleton-instansen av denne
	*/
	static Eventqueue* instance() {
		if (!_instance) {
			_instance = new Eventqueue();
		}
		return _instance;
	}
	
	/**
	* Navnet som vil bli eksponert som navnet p� denne tr�den
	*/
	virtual string getName() {
		return "Eventqueue";
	}

	/**
	* Legg en event i k�en
	*/
	void queue(Event* e) {
		push(e);
	}

	/**
	* Start hovedloopen
	*/
	void startListener();
	
	/**
	* Be hovedloopen om � stopp etterhvert
	*/
	void stopListener();
		
	/**
	* Registrer en event-handler p� k�en.
	* Registrerte handlere blir kalt med eventer som kommer ut etter ferden gjennom k�en.
	*/
	void registerHandler(EventHandler* handler);
};


}


#endif /* EVENTQUEUE_H_ */
