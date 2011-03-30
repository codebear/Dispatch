/*
 * DispatchModule.cpp
 *
 *  Created on: May 20, 2009
 *      Author: bear
 *
 * Basisklasse for pluggbare moduler i systemet
 */

#include "DispatchModule.h"

namespace dispatch {
namespace module {


DispatchModule::DispatchModule() : 
	NameTimeTaggingOutputSet(cout.rdbuf(), clog.rdbuf(), cerr.rdbuf(), cerr.rdbuf()),
	queue_provider(0)
/*	out(this, cout.rdbuf()), 
	log(this, clog.rdbuf()),
	err(this, cerr.rdbuf()),
	dbg(this, cerr.rdbuf())*/ {
}

DispatchModule::~DispatchModule() {

}

string DispatchModule::getName() {
	return this->getModuleName();
}

QueueProvider* DispatchModule::getQueueProvider() {
	return queue_provider;
}

EventQueue* DispatchModule::getEventQueue() {
	return getQueueProvider()->getEventQueue();
}

void DispatchModule::setQueueProvider(QueueProvider* prov) {
	queue_provider = prov;
}

config::GConfigNodeVisitor *DispatchModule::getConfigVisitor()
{
	return new DispatchModuleConfigVisitor(this);
}

void DispatchModuleConfigVisitor::node(config::GConfigNode* node) {
	node->visitChildren(this);
}

}} // end namespace

