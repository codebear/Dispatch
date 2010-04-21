/*
 * FifoStreamListener.cpp
 *
 *  Created on: May 21, 2009
 *      Author: bear
 */

#include "FifoStreamListener.h"
#include "../../util/FileStat.h"

using namespace dispatch::util;
namespace dispatch {
namespace module {
namespace fifo {
FifoStreamListener::FifoStreamListener() {

}

FifoStreamListener::FifoStreamListener(string fifo_f) : fifo(fifo_f) {

}

FifoStreamListener::~FifoStreamListener() {
	if (pipe.is_open()) {
		pipe.close();
	}
}

    bool FifoStreamListener::initStream() {
    	if (!fifo.length()) {
    		err << "Mangler fifo" << endl;
			return false;
    	}

    	err << "Opening fifo : " << fifo << endl;
    	pipe.open(fifo.c_str(), ios::in);
    	err << "Opened." << endl;
    	return pipe.is_open();
    }

    bool FifoStreamListener::hasValidFifo() {
    	FileStat stat(fifo);
    	return stat.isFifo();
    }

    void FifoStreamListener::openStream() {
    	err << "Attempting to open " << fifo << endl;
    	pipe.open(fifo.c_str(), ios::in);
    	err << "Done." << endl;
    }

    bool FifoStreamListener::validStream() {
    	if (!pipe.is_open()) {
    		return false;
    	}
    	if (pipe.eof()) {
    		return false;
    	}
    	return true;
    }

    void FifoStreamListener::closeStream() {
    	pipe.close();
    }

    void FifoStreamListener::setFifo(string fifo_f) {
    	fifo = fifo_f;
    }
    
    string FifoStreamListener::getName() {
    	return "FifoStreamListener";
    }


    istream*  FifoStreamListener::getInputStream() {
    	return &pipe;
    }

}}}
