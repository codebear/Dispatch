/*
 * FifoStreamListener.h
 *
 *  Created on: May 21, 2009
 *      Author: bear
 */

#ifndef FIFOSTREAMLISTENER_H_
#define FIFOSTREAMLISTENER_H_
#include "../../core/StreamEventListener.h"
#include <fstream>
#include <iostream>

using namespace dispatch::core;
namespace dispatch {
namespace module {
namespace fifo {
/**
* Lone-thread fifo-event-listener
*/
class FifoStreamListener : public StreamEventListener {
	/**
	* Input-str�mmen vi skal lese eventer fra
	*/
	ifstream pipe;
	/**
	* Path til fifo-en som man �pner str�mmen fra
	*/
	string fifo;
public:
	FifoStreamListener();
	/**
	* Path til fifo som argument
	*/
	FifoStreamListener(string fifo_f);
	virtual ~FifoStreamListener();
	/**
	* Sett stien til fifoen som skal leses fra
	*/
	void setFifo(string fifo);
	
	/**
	* Navn p� tr�den
	*/
	virtual string getName();
	
	/**
	* Initaliser input str�m fra fifo
	*/
	virtual bool initStream();
	
	/**
	* Er fifoen en gyldig fifo p� disken?
	*/
	virtual bool hasValidFifo();
	
	/**
	* �pne streamen p� nytt
	*/
	virtual void openStream();
	
	/**
	* Er streamen gyldig
	*/ 
	virtual bool validStream();
	
	/**
	* Lukk str�mmen
	*/
	virtual void closeStream();
	
	/**
	* Hent peker til input str�m
	*/
	virtual istream* getInputStream();
};

}}}

#endif /* FIFOSTREAMLISTENER_H_ */
