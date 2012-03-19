/*
 * node_base.cc
 *
 *  Created on: May 24, 2009
 *      Author: bear
 */

#include "node_base.h"

using namespace std;
using namespace dispatch::config::filter;

namespace dispatch {
namespace config {

GConfigAutoreleasePool* GConfigAutoreleasePool::instance = NULL;

GConfigNode::GConfigNode(const char* classname) : 
	_classname(classname), 
	parent(NULL),
	_used(0),
	_error(0)
{
	GConfigAutoreleasePool::getInstance()->add(this);
//			std::cout << "Nytt " << classname << " @ " << this << std::endl;
}

GConfigNode::GConfigNode(const char* classname, location l) : 
	_classname(classname), 
	parent(NULL),
	_used(0),
	config_loc(l)
{
	string* fname = config_loc.begin.filename;
	if (fname) {
		filename = *fname;
	}
	config_loc.begin.filename = config_loc.end.filename = &filename;
}

GConfigNode::~GConfigNode() {
	GConfigAutoreleasePool::getInstance()->remove(this);
}

void GConfigNode::removeFromAutoReleasePool() {
	GConfigAutoreleasePool::getInstance()->remove(this);
}

void GConfigNode::setParentNode(GConfigNode* p) {
	parent = p;
}

GConfigNode* GConfigNode::getParentNode() {
	return parent;
}

int GConfigNode::getLevel() {
	if (parent) {
		return parent->getLevel()+1;
	}
	return 0;
}
string GConfigNode::getNodeName() {
	return string();
}

NodeIdent GConfigNode::getNodeIdent() {
	return NodeIdent();
}


GConfigNode* GConfigNode::findParentByFilter(filter::NodeFilter* f, int level) {
	if (level && f->accept(this)) {
		return this;
	}
	GConfigNode* par = getParentNode();
	if (par) {
		return par->findParentByFilter(f, level+1);
	}
	return NULL;
}


NodeIdent GConfigNode::getFullNodeIdent() {
	NodeIdent id = getNodeIdent();
	NodeIdent pident;
	GConfigNode* parent = getParentNode();
	if (parent != NULL) {
		pident = parent->getFullNodeIdent();
		return pident.concat(id);
	}
	return id;

}

GConfigNodeList GConfigNode::findNodesByName(const string& name, int type) {
	GConfigNodeSearchVisitor vst (new And(new Name(name), new Type(type)));
	visit(&vst);
	return vst.getNodeList();
}

GConfigNodeList GConfigNode::findNodesByFilter(filter::NodeFilter* f) {
	GConfigNodeSearchVisitor vst(f);
	visit(&vst);
	return vst.getNodeList();
}




std::ostream& operator<< (std::ostream& os, GConfigNode& ptr) {
	return ptr.operator<<(os);
}

std::ostream& operator<< (std::ostream& os, GConfigNode* ptr) {
	return ptr->operator<<(os);
}

}} // end namespace
