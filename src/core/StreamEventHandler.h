
#include <string>
#include "EventQueue.h"

#ifndef _STREAM_EVENT_HANDLER_H
#define _STREAM_EVENT_HANDLER_H 1

namespace dispatch { namespace core {

/**
* Handler for mottak av eventer i stream-form, disse konkretiseres og lever til 
* spesifisert kø.
* Metodene kan ikke intermixes fra forskjellige streamer på en instans av dette objektet.
*/
class StreamEventHandler {
	Eventqueue* queue;
	Event* working_event;
	string rest_buffer;
public:
	StreamEventHandler();
	
	/**
	* Konstruer med en kø for leveranse
	*/
	StreamEventHandler(Eventqueue*);
	
	/**
	* Peker til kø eventene skal leveres til
	*/
	void setEventQueue(Eventqueue*);
	
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

}} // end namespace

#endif
