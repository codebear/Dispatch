
#include <string>
#include "EventQueue.h"
#include "../config/NodeIdent.h"
#include "QueueProvider.h"

#ifndef _STREAM_EVENT_HANDLER_H
#define _STREAM_EVENT_HANDLER_H 1


namespace dispatch { namespace core {


/**
* @TODO splitt den her i 2, sÃ¥nn at man ikkje fÃ¥r kompila en handlier som mangel queue.
* En variant som selv e ansvarlig for kÃ¸en (abstrakt)
* og en som throw en exception dersom den ikkje har fÃ¥tt en kÃ¸, og den bi bruk fÃ¸r.
*/

class StreamEventHandler {
	Event* working_event;
	string rest_buffer;
	config::NodeIdent base_ident;
public:
//	StreamEventHandler();
	
	/**
	* Konstruer med en kø for leveranse
	*/
	StreamEventHandler();
	
	
	StreamEventHandler(config::NodeIdent&);
	/**
	*
	*/
	
	/**
	* Hent ut køn som skal leses fra
	*/
	virtual EventQueue* getEventQueue() = 0;
	
	/**
	* Håndter en chunk av en stream. Kan være bare en del av en eller flere eventer.
	* Kan også være deler av en linje.
	*/
	void handleStreamChunk(string chunk, bool complete = false);

	/**
	* Håndter en konkret linje av en stream.
	*/
	void handleStreamLine(string line, bool complete = false);
	
	/**
	* Håndter en string med det som på forhånd er definert en multilinje event som er komplett.
	*/
	void handleCompleteEvent(string raw_event);

};

class StreamEventQueueProvidingHandler : public StreamEventHandler {
	QueueProvider* queue_provider;
public:
	StreamEventQueueProvidingHandler(QueueProvider*);
	
	StreamEventQueueProvidingHandler(QueueProvider*, config::NodeIdent&);

	EventQueue* getEventQueue();
};

/**
* Handler for mottak av eventer i stream-form, disse konkretiseres og lever til 
* spesifisert kÃ¸.
* Metodene kan ikke intermixes fra forskjellige streamer pÃ¥ en instans av dette objektet.
*/
class StreamEventQueueHandler : public StreamEventHandler {
	EventQueue* queue;

public:
	/**
	*
	*/
	StreamEventQueueHandler(EventQueue*);

	/**
	*
	*/
	StreamEventQueueHandler(EventQueue*, config::NodeIdent&);

	/**
	* Peker til kø eventene skal leveres til
	*/
	void setEventQueue(EventQueue*);
	

	EventQueue* getEventQueue();

};

}} // end namespace

#endif
