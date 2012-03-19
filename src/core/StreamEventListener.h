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
	* Instansier med et tr�d-navn
	*/
//	StreamEventListener(string name);
	
	StreamEventListener(string name, StreamEventHandler*);
	
	StreamEventListener(StreamEventHandler*);
		
	virtual ~StreamEventListener();

	/**
	* Initialiser in-str�mmen
	*/
	virtual bool initStream() = 0;

	/**
	* F� en peker til inn-str�mmen
	*/
	virtual istream* getInputStream() = 0;
	
	/**
	* Har man en gyldig str�m?
	*/
	virtual bool validStream() = 0;

	/**
	* �pne str�mmen
	*/
	virtual void openStream() = 0;

	/**
	* Lukk str�mmen
	*/
	virtual void closeStream() = 0;

	/**
	* Kj�r lytteren
	*/
	virtual void run();
	
	/**
	* Les inn s� mange eventer som mulig fra input str�mmen. Kalles repetivt fra main-loop s� lenge isRunning (fra Thread) er sann 
	*/
	void pullEvents();
};

}} // end namespace

#endif /* SOCKETEVENTLISTENER_H_ */
