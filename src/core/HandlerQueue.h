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
	EventHandler* handler;
public:

	HandlerQueue(EventHandler* handler);
	

	/**
	* Navnet som vil bli eksponert som navnet på denne tråden
	*/
	virtual string getName();

	/**
	* Legg en event i køen
	*/
	void queue(Event* e);

	void handleEvent(Event* e);

	/**
	* Start hovedloopen
	*/
	void startListener();
	
	/**
	* Be hovedloopen om å stopp etterhvert
	*/
	void stopListener();
		
};
