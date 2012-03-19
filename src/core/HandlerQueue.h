#include "../util/BlockingQueue.h"

/**
* Event-k�en
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
	* Navnet som vil bli eksponert som navnet p� denne tr�den
	*/
	virtual string getName();

	/**
	* Legg en event i k�en
	*/
	void queue(Event* e);

	void handleEvent(Event* e);

	/**
	* Start hovedloopen
	*/
	void startListener();
	
	/**
	* Be hovedloopen om � stopp etterhvert
	*/
	void stopListener();
		
};
