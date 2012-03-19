/*
 * Socketeventlistener.h
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */

#ifndef SOCKETEVENTLISTENER_H_
#define SOCKETEVENTLISTENER_H_

#include <iostream>
#include "../util/Thread.h"
#include "StreamEventHandler.h"
#include "../util/NameTimeInserter.h"
#include "../config/NodeIdent.h"

using namespace std;
using namespace dispatch::util;
using namespace dispatch::config;

namespace dispatch {
namespace core {

/**
* Basisklasse for event-listenere som leser fra en istream
*/


class StreamEventListener : 
		public Thread, public NameTimeTaggingOutputSet 
{
protected:
	StreamEventHandler* handler;
public:
//	StreamEventListener();
	/**
	* Instansier med et tråd-navn
	*/
//	StreamEventListener(string name);
	
	StreamEventListener(string name, StreamEventHandler*);
	
	StreamEventListener(StreamEventHandler*);
		
	virtual ~StreamEventListener();

	/**
	* Initialiser in-strømmen
	*/
	virtual bool initStream() = 0;

	/**
	* Få en peker til inn-strømmen
	*/
	virtual istream* getInputStream() = 0;
	
	/**
	* Har man en gyldig strøm?
	*/
	virtual bool validStream() = 0;

	/**
	* Åpne strømmen
	*/
	virtual void openStream() = 0;

	/**
	* Lukk strømmen
	*/
	virtual void closeStream() = 0;

	/**
	* Kjør lytteren
	*/
	virtual void run();
	
	/**
	* Les inn så mange eventer som mulig fra input strømmen. Kalles repetivt fra main-loop så lenge isRunning (fra Thread) er sann 
	*/
	void pullEvents();
};

}} // end namespace

#endif /* SOCKETEVENTLISTENER_H_ */
