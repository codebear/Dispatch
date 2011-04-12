
#include "NodeIdent.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <boost/algorithm/string.hpp>


namespace dispatch { namespace config {

NodeIdent::NodeIdent() {

}

NodeIdent::NodeIdent(string i) {
	// @TODO create path of input ident
	boost::split(path, i, boost::is_any_of("."));
}

string NodeIdent::getPathStr(const string sep) const {
		uint c = 0;
		stringstream res;
		for(uint i = 0; i < path.size(); ++i) {
			if (c++) {
				res << sep;
			}
			res << path[i];
		}
		return res.str();
	}
	
string NodeIdent::getPathStr() const {
	return getPathStr(".");
}


NodeIdent NodeIdent::concat(NodeIdent& post) {
	NodeIdent id;
	vector<string>::iterator i;
	for(i = path.begin(); i < path.end(); i++) {
		id.path.push_back(*i);
	}
	for(i = post.path.begin(); i < post.path.end(); i++) {
		id.path.push_back(*i);
	}
	return id;
}


bool NodeIdent::equals(const NodeIdent& i) {
	string a = getPathStr();
	string b = i.getPathStr();
	return a == b;
}

bool NodeIdent::operator==(const NodeIdent& i) {
	return equals(i);
}

bool NodeIdent::operator==(const string& s) {
	NodeIdent x(s);
	return this->equals(x);
}

bool NodeIdent::contains(NodeIdent& child) {
	uint par_size = path.size();
	uint ch_size = child.path.size();
	if (par_size > ch_size) {
		return false;
	}
	for(uint i; i < par_size; i++) {
		if (path[i] != child.path[i]) {
			return false;
		}
	}
	return true;
}

bool NodeIdent::contained(NodeIdent& parent) {
	return parent.contains(*this);
}

ostream &operator<<(ostream& s, NodeIdent& i)
{
	s << i.getPathStr();
	return s;
}


}} // end namespace