/*
 * FifoEventSourceModule.h
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */

#ifndef INETEVENTSOURCEMODULE_H_
#define INETEVENTSOURCEMODULE_H_
#include "../DispatchModule.h"
#include "InetStreamListener.h"
#include <vector>

namespace dispatch {
namespace module {
namespace inet {

class InetEventSourceEntry {
public: 
	vector<string> ports;
	vector<string> hosts;
	NodeIdent id;
	bool initialized;
};

/**
* Modul som kan lese eventer fra en inet-socket
*/
class InetEventSourceModule : public DispatchModule {
	/**
	* listener handle å lese fra
	*/
   	vector<InetStreamListener*> inet_listeners;
   	
   	vector<InetEventSourceEntry> config_entries;

public:
	InetEventSourceModule();

	virtual ~InetEventSourceModule();

	/**
	* Modul-navn
	*/
	virtual string getModuleName() {
		return "InetEventSource";
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
	* Initialiser en og en
	*/
	bool initializeEntry(InetEventSourceEntry& e);
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

#endif /* INETEVENTSOURCEMODULE_H_ */
