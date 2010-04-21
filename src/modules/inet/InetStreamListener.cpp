/*
 * FifoStreamListener.cpp
 *
 *  Created on: May 21, 2009
 *      Author: bear
 */

#include "InetStreamListener.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <error.h>
#include <sstream>
#include "../../util/FDMonitor.h"


using namespace dispatch::util;
namespace dispatch {
namespace module {
namespace inet {

InetStreamListener::InetStreamListener() : 
	module(NULL) {

}

InetStreamListener::InetStreamListener(DispatchModule* mod, string adr, string port) :
	module(mod),
	inet_addr(adr), 
	inet_port(port) {
}

InetStreamListener::InetStreamListener(DispatchModule* mod, string adr, int port) : 
	module(mod),
	inet_addr(adr) {
	std::stringstream tmp;
	tmp << port;
	inet_port = tmp.str();
}




InetStreamListener::~InetStreamListener() {

}

    bool InetStreamListener::initStream() {
    	int errcnt = 0;
    	int succcnt = 0;
    	
		/**
		* Med inspirasjon fra 
		* http://beej.us/guide/bgnet/output/html/multipage/syscalls.html
		*/
		
		struct ::addrinfo hints, *res_list, *res;

		// first, load up address structs with getaddrinfo():

		memset(&hints, 0, sizeof hints);
		
		hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
		const char* adr = NULL;
		/**
		* Bruk adresse hvis spesifisert
		*/
		if (inet_addr.length()) {
			adr = inet_addr.c_str();
		}
		/**
		* Port-spesifikasjon
		*/
		const char* port = inet_port.c_str();
		
		/**
		* Fyll structer
		*/
		int adr_ret = getaddrinfo(adr, port, &hints, &res_list);
		if (adr_ret != 0) {
			perror("address resolution failed");
			return false;
		}
#define MAX_HOST_LEN 2048
		char hostname[MAX_HOST_LEN] = "";
		for(res = res_list; res != NULL; res = res->ai_next) {
			/**
			* Først, skriver vi litt output om hva vi har funnet
			*/
			module->out << "INET " << endl;
			unsigned short port_no = 0;
			switch(res->ai_family) {
				case AF_INET:
					module->out << "IPv4: ";
					do {
						struct sockaddr_in* adr4 = (struct sockaddr_in*) res->ai_addr;
						port_no = ntohs(adr4->sin_port);
					} while(0);
					break;
				case AF_INET6:
					module->out << "IPv6: ";
					do {
						struct sockaddr_in6* adr6 = (struct sockaddr_in6*) res->ai_addr;
						port_no = ntohs(adr6->sin6_port);
					} while(0);
					break;
				default:
					module->out << "Unknown family: ";
					break;
			}
			int name_res = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, MAX_HOST_LEN, NULL, 0, 0);
			if (name_res != 0) {
				module->err << "error in getnameinfo " << gai_strerror(name_res) << endl;
				continue;
			}
			if (*hostname) {
				module->out << "hostname: [" << hostname << "] ";
			}
			if (port) {
				module->out << "port: " << port_no;
			}
			
			module->out << endl;
			
			/**
			* Så henger vi oss på nettet
			*/

			/**
			* Lag socket-filedescriptor
			*/
			int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if (socket_fd == -1) {
				module->err << "Error opening socket: " << strerror(errno) << endl;
				errcnt++;
				continue;
			}

			/**
			* Bind til nettverket
			*/
			int b_ret = bind(socket_fd, res->ai_addr, res->ai_addrlen);
			if (b_ret == -1) {
				module->err << "Error binding socket: " << strerror(errno) << endl;
				errcnt++;
				continue;
			}
			
			module->out << "Bunded til socket" << endl;
			
			int l_ret = listen(socket_fd, 10);
			if (l_ret != 0) {
				module->err << "Error listening socket" << strerror(errno) << endl;
				errcnt++;
				continue;
			}
			sockets.push_back(socket_fd);
			succcnt++;		
		} // end foreach addr
		freeaddrinfo(res_list);
		if (succcnt) {
			return true;
		}
		return false;
    }


void InetStreamListener::openStream() {
/*    	cerr << "Attempting to open " << fifo << endl;
    	pipe.open(fifo.c_str(), ios::in);
    	cerr << "Done." << endl;
*/
}

bool InetStreamListener::validStream() {
	return false;
/*    	if (!pipe.is_open()) {
    		return false;
    	}
    	if (pipe.eof()) {
    		return false;
    	}
    	return true;
*/
}

string InetStreamListener::getName() {
	return "InetStreamListener";
}

void InetStreamListener::closeStream() {
//    	pipe.close();
}


istream*  InetStreamListener::getInputStream() {
	return NULL;
//    	return &pipe;
}

void InetStreamListener::attachToMonitor(FDMonitor::type& mon) {
	vector<int>::iterator it;
	for(it = sockets.begin(); it != sockets.end(); it++) {
		mon.registerFD(
			*it, 
			POLLIN, 
			new TSpecificFunctor<InetStreamListener, FDMonitorEvent*>(this, &InetStreamListener::handleSocketEvent)
		);
	}
	monitor = &mon;
}

void InetStreamListener::handleSocketEvent(FDMonitorEvent* evnt) {
	int event = evnt->event;
	string names = FDEvent::getEventNames(event);
//	module->out << "InetStreamListener: Received socket event: " << names << " for socket: " << evnt->fd << endl;
	if (evnt->event & POLLIN) {
			// Vi mottok en tilkobling
			this->acceptConnection(evnt->fd);
	}
}

void InetStreamListener::acceptConnection(int socket) {
	InetStreamConnection* conn = new InetStreamConnection(this, monitor);
	conn->socket = accept(socket, &(conn->remote_addr), &(conn->remote_len));
	if (conn->socket == -1) {
//		module->err << "Accept socket failed: " << strerror(errno) << endl;
		delete conn;
		return;
	}
	if (monitor) {
		monitor->registerFD(
			conn->socket, 
			POLLIN, 
			new TSpecificFunctor<InetStreamConnection, FDMonitorEvent*>(conn, &InetStreamConnection::handleSocketEvent)
		);
	}
	connections.push_back(conn);
}

}}} // end namespace
