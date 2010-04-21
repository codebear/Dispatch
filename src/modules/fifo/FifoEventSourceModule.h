/*
 * FifoEventSourceModule.h
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */

#ifndef FIFOEVENTSOURCEMODULE_H_
#define FIFOEVENTSOURCEMODULE_H_
#include "../DispatchModule.h"
#include "FifoStreamListener.h"

namespace dispatch {
namespace module {
namespace fifo {

/**
* Modul som kan lese eventer fra en fifo-node i filsystemet
*/
class FifoEventSourceModule : public DispatchModule {
	/**
	* listener handle å lese fra
	*/
   	FifoStreamListener fifo_listener;
   	
   	/**
   	* Sti til fifoen som skal leses fra
   	*/
   	string fifo_path;

public:
	FifoEventSourceModule();

	virtual ~FifoEventSourceModule();

	/**
	* Modul-navn
	*/
	virtual string getModuleName() {
		return "FifoEventSource";
	}
	
	/**
	* JA, er event-kilde
	*/
	virtual bool isEventSource() {
		return true;
	}

	/**
	* Er ikke event-handler
	*/
	virtual bool isEventHandler() {
		return false;
	}

	/**
	* Er service-handler
	*/
	virtual bool isServiceHandler() {
		return false;
	}
	
	/**
	* Motta konfigurasjon
	*/
	virtual bool scanConfigNode(GConfigNode* node);

	/**
	* 1.step initialization
	*/
	virtual bool preInitialize();

	/**
	* Kickstart igang
	*/
	virtual bool startup();

	/**
	* Steng ned
	*/
	virtual bool shutdown();
};

}}}

#endif /* FIFOEVENTSOURCEMODULE_H_ */
