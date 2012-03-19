/*
 * FifoStreamListener.h
 *
 *  Created on: May 21, 2009
 *      Author: bear
 */



#ifndef FIFOSTREAMLISTENER_H_
#define FIFOSTREAMLISTENER_H_

#include "../../core/StreamEventListener.h"
#include "../DispatchModule.h"
#include "../../util/FDMonitor.h"
#include "InetStreamConnection.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace dispatch::util;
using namespace dispatch::core;
namespace dispatch {
namespace module {
namespace inet {
class InetStreamConnection;
/**
* Lone-thread inet-event-listener
*/
class InetStreamListener : public StreamEventListener {
	string inet_addr;
	string inet_port;
	
	vector<int> sockets;
//	DispatchModule* module;
	FDMonitor::type* monitor;
	vector<InetStreamConnection*> connections;
//	vector<int> listeners;
	friend class InetStreamConnection;
public:
//	InetStreamListener();

	/**
	* Instansier en inet-listener med en kilde-module peker, og adresse og port den skal lytte på
	*/
//	InetStreamListener(DispatchModule* mod, string addr, int port);
	InetStreamListener(StreamEventHandler*, string addr, int port);

	/**
	* Instansier en inet-listener med en kilde-module peker, og adresse og port den skal lytte på
	*/
//	InetStreamListener(DispatchModule* mod, string addr, string port);
	InetStreamListener(StreamEventHandler*, string addr, string port);
	
	virtual ~InetStreamListener();
	
	/**
	* Navnet på tråden
	*/
	virtual string getName();
	
	/**
	* Initaliser input strøm fra fifo
	*/
	virtual bool initStream();
	
	
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
	
	/**
	* Håndter denne gjennom global fd-tråd
	*/
	void attachToMonitor(FDMonitor::type& monitor);
	
	/**
	* Event fra socketen vi lytter på
	*/
	void handleSocketEvent(FDMonitorEvent* evnt);
	
	/**
	* Godta en tilkobling
	*/
	void acceptConnection(int socket);
	
	/**
	* Hent ut event-queue
	*/
//	virtual EventQueue* getEventQueue();
};

}}}

#endif /* INETSTREAMLISTENER_H_ */
