/*
 * eventqueue.C
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */
#include <cstdio>
#include <string>
#include <iostream>
#include "EventQueue.h"
using namespace std;
using namespace dispatch::util;
namespace dispatch {

string Event::getParameter(string key) {
		return parameters[key];
}

void Event::setParameter(string key, string value) {
//	cout << "Event::setParameter(" << key << ", " << value << "); " << endl;
	parameters[key] = value;
//	cout << "Satt parameter... " << endl;
}

map<string, string> Event::getParameters() {
	return parameters;
}

bool Event::isEmpty() {
	return parameters.empty();
}	

void Event::clearParameter(string key) {
	parameters.erase(key);
}

Eventqueue::Eventqueue() : 
	NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) {

}


void Eventqueue::startListener() {
	err << "Event listener starting..." << endl;
	Event* evnt;
	while(front(evnt) == 0) {
		err << "Received event.." << endl;
		/**
		* Enn så lenge så handler vi bare eventen til alle handlerne. Vi får lage filtre etterhvert
		*/
		vector<EventHandler*>::iterator h_iter;
		for(h_iter = handlers.begin(); h_iter != handlers.end(); h_iter++) {
			(*h_iter)->handleEvent(evnt);
		}
		delete evnt;
		pop();
	}
}

void Eventqueue::stopListener() {
	signal_done();
}
/*
void Eventqueue::run() {
	startListener();
}

void Eventqueue::stop() {
	signal_done();
	this->Thread::stop();
}

*/
Eventqueue* Eventqueue::_instance = NULL;


void Eventqueue::registerHandler(EventHandler* handler) {
	handlers.push_back(handler);
}

} // end namespace
