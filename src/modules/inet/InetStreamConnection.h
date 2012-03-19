


#ifndef INET_STREAM_CONNECTION_H
#define INET_STREAM_CONNECTION_H 1

#include "InetStreamListener.h"
#include "../../util/FDMonitor.h"
#include "sys/socket.h"
#include "../../core/StreamEventHandler.h"


using namespace dispatch::util;
using namespace dispatch::core;

namespace dispatch { namespace module { namespace inet {

class InetStreamListener;

/**
* Tilkobling til en inet-stream event kilde
*/
class InetStreamConnection {
	InetStreamListener* listener;
	FDMonitor::type* monitor;
//	vector<string> lines;
//	StreamEventHandler handler;
	
public:
	/**
	* Adressen til event-kilden
	*/
	struct sockaddr remote_addr;
	/**
	* Lengde p� host-navnet til event-kilden
	*/
	socklen_t remote_len;
	
	/**
	* Socketen klienten er koblet p�
	*/
	int socket;
	
	/**
	* Instansier med listeneren som fikk tilkoblingen, og en file-descriptor overv�ker som kaller oss n�r noe skjer
	*/
	InetStreamConnection(InetStreamListener* list); //, FDMonitor::type* mon);

	/**
	* Heng seg på fd-monitoren
	*/
	void attach(FDMonitor::type* monitor);	
	/**
	* Event p� socketen som m� h�ndteres
	*/
	void handleSocketEvent(FDMonitorEvent* evnt);
	
	/**
	* Les fra socketen
	*/
	void readFromSocket();
	
	/**
	* Kalles n�r klienten kobler fra
	*/
	void disconnected();
	
};

}}} // namespace

#endif

