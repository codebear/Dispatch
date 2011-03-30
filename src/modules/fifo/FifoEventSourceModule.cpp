/*
 * FifoEventSourceModule.cpp
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */

#include "FifoEventSourceModule.h"

using namespace dispatch::config::filter;

namespace dispatch {
namespace module {
namespace fifo {

	FifoEventSourceModule::FifoEventSourceModule() {

	}

	FifoEventSourceModule::~FifoEventSourceModule() {

	}
		
    bool FifoEventSourceModule::preInitialize() {
    	out << "Fifo: preInitialize..." << endl;
  		string path = "events";
    	if (fifo_path.length()) {
	    	fifo_listener.setFifo(fifo_path);
	    }
    	return fifo_listener.hasValidFifo();
    }

    bool FifoEventSourceModule::shutdown() {
		out << "Stenger ned FifoEventSourceModule" << endl;
    	fifo_listener.stop();
    	return !fifo_listener.isStopped();
    }

	bool FifoEventSourceModule::startup() {
		out << "Starter opp FifoEventSourceModule" << endl;
    	fifo_listener.start();
    	return fifo_listener.isRunning();
	}

	bool FifoEventSourceModule::scanConfigNode(GConfigNode* node) {
	
		out << "FIFO: Fikk en konfig-node.." << endl;
		And find_socket(
			new Ident("socket", true), 
			new Type(GConfig::VARIABLE),
			new ContainedIn(new Name("listener"))
		);
		GConfigNodeList treff = node->findNodesByFilter(&find_socket);
		GConfigNodeList::iterator i(treff.begin());
		out << "Fant følgende fornuftig:" << endl;
		int cnt = 0;
		bool retval = false;
		while ( i != treff.end() ) {
			/**
			* Vi kan "trykt" type-caste her, fordi ut fra filter-regelene
			* vi har definert ovenfår _MÅ_ dette være en GConfigVariableStatement.
			* Dette forutsetter selvfølgelig at filtrene fungerer som de skal.
			*/ 
			
			GConfigVariableStatement* val_stat = dynamic_cast<GConfigVariableStatement*> (*i);
			if (!val_stat) {
				/**
				* Bare for sikkerhetsskyld, så slipper vi en segfault
				*/
				continue;
			}
			GConfigScalarVal* val = val_stat->getValue();
			if (val) {
				out << "Fant socket-sti: " << val->getStringValue() << endl;
				fifo_path = val->getStringValue();
				val_stat->used(1);
				retval = true;
			} else {
				out << "Fikk noe rart: " << *i << endl;
			}
			++i;
			cnt++;
		}
		out << cnt << " treff" << endl;
		if (cnt > 0) {
			node->used(1);
		}
		return retval;
	}

}}}

extern "C" {

	extern dispatch::module::DispatchModule* initialize_dispatch_module() {
		return new dispatch::module::fifo::FifoEventSourceModule();
	}

	extern void destroy_dispatch_module(dispatch::module::DispatchModule* ptr) {
		delete ptr;
	}

}
