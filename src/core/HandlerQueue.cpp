#include("HandlerQueue.h")

HandlerQueue::HandlerQueue(EventHandler* _handler) :
	NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) {

{
	handler = _handler;
}
	

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
string HandlerQueue::getName() {
	return "HandlerQueue";
}


	/**
	* Start hovedloopen
	*/
void HandlerQueue::startListener() {
	err << "HandlerQueue starting..." << endl;
	Event* evnt;
	while(front(evnt) == 0) {
		err << "HQ: Received event.." << endl;
		/**
		* Enn så lenge så handler vi bare eventen til alle handlerne. Vi får lage filtre etterhvert
		*/
		handler->handlEvent(evnt);
/*		vector<EventHandler*>::iterator h_iter;
		for(h_iter = handlers.begin(); h_iter != handlers.end(); h_iter++) {
			/**
			* Sender av gårde til handler
			* /
			err << "Sender ut til handler." << endl;
			(*h_iter)->handleEvent(evnt);
		}
		delete evnt;*/
		pop();
	}

}
	
/**
* Be hovedloopen om å stopp etterhvert
*/
void HandlerQueue::stopListener() {
	signal_done();
}
