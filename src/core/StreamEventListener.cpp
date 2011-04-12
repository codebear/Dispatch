/*
 * StreamEventListener.cpp
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */

#include "StreamEventListener.h"
#include <vector>
#include <string>
#include <stdexcept>
#include "EventQueue.h"

using namespace std;
using namespace dispatch::config;


namespace dispatch { namespace core {

	StreamEventListener::StreamEventListener() : 
		NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) 
	{
		handler = new StreamEventHandler();
	}
	
	StreamEventListener::StreamEventListener(string name) : 
		Thread(name),
		NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) 
	{
		handler = new StreamEventHandler();	
	}
	
	StreamEventListener::StreamEventListener(string name, EventQueue* queue) : 
		Thread(name),
		NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) 
	{
		handler = new StreamEventHandler(queue);	
	}
	
	StreamEventListener::StreamEventListener(string name, EventQueue* queue, NodeIdent& id) :
		Thread(name),
		NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf())
	{
		handler = new StreamEventHandler(queue, id);
	}
		
	
	StreamEventListener::StreamEventListener(EventQueue* queue) : 
		NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) 
	{
		handler = new StreamEventHandler(queue);	
	}
	
	StreamEventListener::StreamEventListener(EventQueue* queue, NodeIdent id)
	{
		handler = new StreamEventHandler(queue, id);
	}

	StreamEventListener::~StreamEventListener() {
		delete handler;
	}
	
    void StreamEventListener::run() {
    	while(isRunning()) {
    		if (!validStream()) {
				err << "Reopening stream" << endl;
    			openStream();
    		}
    		if (validStream()) {
				err << "Pulling events from stream" << endl;
    			pullEvents();
//    			cout << "Socket ended..." << endl;
    		} else {
    			err << "Invalid stream. Waiting Œ while" << endl;
    			Thread::sleep(500);
    		}
    	}
    }

    void StreamEventListener::pullEvents() {
    	string evnt_line;
    	istream* pipe = getInputStream();
		/**
		* Så lenge vi har en gyldig kanal å lese fra
		*/
   		while(validStream() && !pipe->eof()) {
   			getline(*pipe, evnt_line);
   			handler->handleStreamLine(evnt_line);
   		}
   		evnt_line = "";
  		// Sørg for å flushe evnetuelle ting som henger igjen
   		handler->handleStreamLine(evnt_line, true);

    	closeStream();
    }

}} // end namepsace
