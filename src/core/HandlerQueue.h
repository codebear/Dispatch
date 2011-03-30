#include "../util/BlockingQueue.h"

/**
* Event-køen
*/
class HandlerQueue : 
	public util::BlockingQueue<Event>,
	public EventHandler
	public NameTimeTaggingOutputSet 
{
private:
	/**
	* Instans for singleton-patternet
	*/
//	static EventQueue* _instance;
	
//	vector<EventHandler*> handlers;
	EventHandler* handler;
public:

	HandlerQueue(EventHandler* handler);
	

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
	virtual string getName();

	/**
	* Legg en event i køen
	*/
	void queue(Event* e) {
		push(e);
	}

	void handleEvent(Event* e) {
		push(e);
	}

	/**
	* Start hovedloopen
	*/
	void startListener();
	
	/**
	* Be hovedloopen om å stopp etterhvert
	*/
	void stopListener();
		
	/**
	* Registrer en event-handler på køen.
	* Registrerte handlere blir kalt med eventer som kommer ut etter ferden gjennom køen.
	*/
//	void registerHandler(EventHandler* handler);
};
