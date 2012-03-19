#include "HandlerQueue.h"

HandlerQueue::HandlerQueue(EventHandler* _handler) 
//:	NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) 
{

{
	handler = _handler;
}
	

/**
* Navnet som vil bli eksponert som navnet p� denne tr�den
*/
string HandlerQueue::getName() {
	return "HandlerQueue";
}


	/**
	* Start hovedloopen
	*/
void HandlerQueue::startListener() {
	err() << "HandlerQueue starting..." << endl;
	Event* evnt;
	while(front(evnt) == 0) {
		err() << "HQ: Received event.." << endl;
		/**
		* Enn s� lenge s� handler vi bare eventen til alle handlerne. Vi f�r lage filtre etterhvert
		*/
		handler->handlEvent(evnt);
		pop();
	}

}

	void HandlerQueue::queue(Event* e) {
		push(e);
	}

	void HandlerQueue::handleEvent(Event* e) {
		push(e);
	}

	
/**
* Be hovedloopen om � stopp etterhvert
*/
void HandlerQueue::stopListener() {
	signal_done();
}
