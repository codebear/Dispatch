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

bool Event::hasParameter(string key) {
	return parameters.count(key) > 0;
}

void Event::setSrcNodeIdent(config::NodeIdent& id) {
	src_ident = id;
}

config::NodeIdent& Event::getSrcNodeIdent() {
	return src_ident;
}

EventQueue::EventQueue() 
//	: NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) 
{

}


void EventQueue::startListener() {
	err() << "Event listener starting..." << endl;
	Event* evnt;
	vector<EventHandler*>::iterator h_iter;
	while(front(evnt) == 0) {
		err() << "Received event.." << endl;
		/**
		* Enn så lenge så handler vi bare eventen til alle handlerne. Vi får lage filtre etterhvert
		*/
		for(h_iter = handlers.begin(); h_iter != handlers.end(); h_iter++) {
			/**
			* Sender av gårde til handler
			*/
			err() << "Sender ut til handler." << endl;
			(*h_iter)->handleEvent(evnt);
		}
		delete evnt;
		pop();
	}
}

void EventQueue::stopListener() {
	signal_done();
}

string EventQueue::getName()
{
	return "ThreadedEventQueue";
}

void EventQueue::queue(Event* e) {
	push(e);
}

void EventQueue::queue(Event* e, config::NodeIdent& id) {
	e->setSrcNodeIdent(id);
	push(e);
}


void EventQueue::registerHandler(EventHandler* handler) {
	handlers.push_back(handler);
}

ThreadedEventQueue::ThreadedEventQueue() {

}


} // end namespace
