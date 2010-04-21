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
	* Input-strømmen vi skal lese eventer fra
	*/
	ifstream pipe;
	/**
	* Path til fifo-en som man åpner strømmen fra
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
	* Navn på tråden
	*/
	virtual string getName();
	
	/**
	* Initaliser input strøm fra fifo
	*/
	virtual bool initStream();
	
	/**
	* Er fifoen en gyldig fifo på disken?
	*/
	virtual bool hasValidFifo();
	
	/**
	* Åpne streamen på nytt
	*/
	virtual void openStream();
	
	/**
	* Er streamen gyldig
	*/ 
	virtual bool validStream();
	
	/**
	* Lukk strømmen
	*/
	virtual void closeStream();
	
	/**
	* Hent peker til input strøm
	*/
	virtual istream* getInputStream();
};

}}}

#endif /* FIFOSTREAMLISTENER_H_ */
