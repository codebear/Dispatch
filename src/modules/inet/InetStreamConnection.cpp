#include "InetStreamConnection.h"
#include <sstream>


namespace dispatch { namespace module { namespace inet {

InetStreamConnection::InetStreamConnection(InetStreamListener* l) : //, FDMonitor::type* mon) :
	listener(l),
	monitor(0),
//	handler(l->getEventQueue()),
	remote_len(0)
{
}

void InetStreamConnection::attach(FDMonitor::type* monitor) {
		/**
		* Register the file-descriptor with global fd-monitor with callback method for handling of fd-events
		*/
		monitor->registerFD(
			socket, 
			POLLIN, 
			new TSpecificFunctor<InetStreamConnection, FDMonitorEvent*>(this, &InetStreamConnection::handleSocketEvent)
		);
}


void InetStreamConnection::handleSocketEvent(FDMonitorEvent* evnt) {
	string en = FDEvent::getEventNames(evnt->event);
//	cout << "InetStreamConnection: Socket events: " << en << " fd: " << evnt->fd << endl;
	if (evnt->event & POLLIN) {
		readFromSocket();
	}
}

void InetStreamConnection::readFromSocket() {
	ssize_t bufsize = 1024;
	char buf[bufsize];
	memset(buf, 0, bufsize);
	
	int flags = MSG_DONTWAIT;
	ssize_t size;
	int total_read = 0;
	do {
		stringstream msg_buf;
		size = recv(socket, buf, bufsize, flags);
		if (size == -1) {
			if (errno != EAGAIN) {
				cerr << "Error reading from socket: " << strerror(errno) << endl;
			}
		} else {
			total_read += size;
//			cout << "Read " << size << " bytes: " << buf << endl;
			msg_buf.write(buf, size);
			string msg = msg_buf.str();
			if (msg.length()) {
//				cout << "Handterer chunk[" << msg.length() << ": [" << msg << endl;
				listener->handler->handleStreamChunk(msg);
			}
		}
	} while(size > 0);
	if (total_read == 0) {
		/**
		* Høyst sannsynligvis en disconnect
		*/ 
		disconnected();
		return;
	}
}

void InetStreamConnection::disconnected() {
	if (monitor) {
		monitor->unregisterFD(socket);
	}
}

}}} // end namespace
