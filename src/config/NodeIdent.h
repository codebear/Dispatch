
#ifndef _CORE_IDENT_H_
#define _CORE_IDENT_H_

#include <vector>
#include <string>

using namespace std;
namespace dispatch { namespace config {

class NodeIdent {
public:

	vector<string> path;
	
	NodeIdent();

	NodeIdent(string i);
	
	NodeIdent(vector<string> v);
	
	string getPathStr() const;
	
	string getPathStr(string sep) const;
	
	NodeIdent parent();
	
	NodeIdent concat(NodeIdent& i);
	
	bool equals(const NodeIdent& i);
	
	bool operator==(const NodeIdent& i);
	
	bool operator==(const string& s);
	
	ostream &operator<<(ostream&);

	/**
	* Check if child-ident actually is contained in this ident
	*/
	bool contains(NodeIdent& child);
	
	/**
	* Check if parent-ident is a parent of this ident
	*/
	bool contained(NodeIdent& parent);

};

//ostream &operator<<(ostream& s, Ident& i);

}} // end namespace

#endif