/*
 * FifoEventSourceModule.cpp
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */

#include "InetEventSourceModule.h"

using namespace dispatch::config::filter;

namespace dispatch {
namespace module {
namespace inet {

	InetEventSourceModule::InetEventSourceModule() {

	}

	InetEventSourceModule::~InetEventSourceModule() {

	}
	
    bool InetEventSourceModule::preInitialize() {
    	out << "Inet: preInitialize..." << endl;
//    	if (fifo_path.length()) {
//	    	fifo_listener.setFifo(fifo_path);
//	    }
//    	return fifo_listener.hasValidFifo();
		return true;
    }

    bool InetEventSourceModule::shutdown() {
		out << "Stenger ned InetEventSourceModule" << endl;
//    	fifo_listener.stop();
//    	return !fifo_listener.isStopped();
		return false;
    }

	bool InetEventSourceModule::startup() {
		out << "Starter opp FifoEventSourceModule" << endl;
		vector<InetStreamListener*>::iterator listener;
		for(listener = inet_listeners.begin(); listener != inet_listeners.end(); listener++) {
			(*listener)->initStream();
			(*listener)->attachToMonitor(FDMonitor::instance());
		}
  //  	fifo_listener.start();
//    	return fifo_listener.isRunning();
    	return true;
	}

	bool InetEventSourceModule::scanConfigNode(GConfigNode* node) {
		vector<string> ports = StringVariableHelper::getStrings(node, "port", 1);
		vector<string> hosts = StringVariableHelper::getStrings(node, "bind", 1);
		vector<string>::iterator port;
		vector<string>::iterator host;
		
		bool retval = false;
		for(port = ports.begin(); port != ports.end(); port++) {
			/**
			* @TODO Dette bør vel gjøres i preInitialize?
			*/
			
			/**
			* Hvis ingen hoster, så binder vi til alt
			*/
			if (hosts.empty()) {
				log << "Binder til [*]:" << *port << endl;
				inet_listeners.push_back(new InetStreamListener(this, "", *port));
				retval = true;
			} else {
				for(host = hosts.begin(); host != hosts.end(); host++) {
					log << "Binder til [" << *host << "]:" << *port << endl;
					inet_listeners.push_back(new InetStreamListener(this, *host, *port));
					retval = true;
				}
			}
		}
		return retval;
	}

}}}

extern "C" {

	extern dispatch::module::DispatchModule* initialize_dispatch_module() {
		return new dispatch::module::inet::InetEventSourceModule();
	}

	extern void destroy_dispatch_module(dispatch::module::DispatchModule* ptr) {
		delete ptr;
	}

}
