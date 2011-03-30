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
	* Navnet som vil bli eksponert som navnet p� denne tr�den
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
		* Enn s� lenge s� handler vi bare eventen til alle handlerne. Vi f�r lage filtre etterhvert
		*/
		handler->handlEvent(evnt);
/*		vector<EventHandler*>::iterator h_iter;
		for(h_iter = handlers.begin(); h_iter != handlers.end(); h_iter++) {
			/**
			* Sender av g�rde til handler
			* /
			err << "Sender ut til handler." << endl;
			(*h_iter)->handleEvent(evnt);
		}
		delete evnt;*/
		pop();
	}

}
	
/**
* Be hovedloopen om � stopp etterhvert
*/
void HandlerQueue::stopListener() {
	signal_done();
}
