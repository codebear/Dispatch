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
	NameTimeTaggingOutputSet(cout.rdbuf(), clog.rdbuf(), cerr.rdbuf(), cerr.rdbuf())
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

config::GConfigNodeVisitor *DispatchModule::getConfigVisitor()
{
	return new DispatchModuleConfigVisitor(this);
}

void DispatchModuleConfigVisitor::node(config::GConfigNode* node) {
	node->visitChildren(this);
}

}} // end namespace

