
#include <string>
#include "EventQueue.h"
#include "../config/NodeIdent.h"
#include "QueueProvider.h"

#ifndef _STREAM_EVENT_HANDLER_H
#define _STREAM_EVENT_HANDLER_H 1


namespace dispatch { namespace core {


/**
* @TODO splitt den her i 2, sånn at man ikkje får kompila en handlier som mangel queue.
* En variant som selv e ansvarlig for køen (abstrakt)
* og en som throw en exception dersom den ikkje har fått en kø, og den bi bruk før.
*/

class StreamEventHandler {
	Event* working_event;
	string rest_buffer;
	config::NodeIdent base_ident;
public:
//	StreamEventHandler();
	
	/**
	* Konstruer med en k� for leveranse
	*/
	StreamEventHandler();
	
	
	StreamEventHandler(config::NodeIdent&);
	/**
	*
	*/
	
	/**
	* Hent ut k�n som skal leses fra
	*/
	virtual EventQueue* getEventQueue() = 0;
	
	/**
	* H�ndter en chunk av en stream. Kan v�re bare en del av en eller flere eventer.
	* Kan ogs� v�re deler av en linje.
	*/
	void handleStreamChunk(string chunk, bool complete = false);

	/**
	* H�ndter en konkret linje av en stream.
	*/
	void handleStreamLine(string line, bool complete = false);
	
	/**
	* H�ndter en string med det som p� forh�nd er definert en multilinje event som er komplett.
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
* spesifisert kø.
* Metodene kan ikke intermixes fra forskjellige streamer på en instans av dette objektet.
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
	* Peker til k� eventene skal leveres til
	*/
	void setEventQueue(EventQueue*);
	

	EventQueue* getEventQueue();

};

}} // end namespace

#endif
