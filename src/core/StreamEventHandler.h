
#include <string>
#include "EventQueue.h"

#ifndef _STREAM_EVENT_HANDLER_H
#define _STREAM_EVENT_HANDLER_H 1

namespace dispatch { namespace core {

/**
* Handler for mottak av eventer i stream-form, disse konkretiseres og lever til 
* spesifisert k�.
* Metodene kan ikke intermixes fra forskjellige streamer p� en instans av dette objektet.
*/
class StreamEventHandler {
	Eventqueue* queue;
	Event* working_event;
	string rest_buffer;
public:
	StreamEventHandler();
	
	/**
	* Konstruer med en k� for leveranse
	*/
	StreamEventHandler(Eventqueue*);
	
	/**
	* Peker til k� eventene skal leveres til
	*/
	void setEventQueue(Eventqueue*);
	
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

}} // end namespace

#endif
