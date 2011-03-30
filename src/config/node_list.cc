
#include <string>
#include <iostream>
#include "node_list.h"
#include "node_base.h"
#include "config_nodes.h"
#include <vector>
using namespace std;
using namespace dispatch::config::filter;


namespace dispatch {
namespace config {

namespace filter {

	bool Or::accept(GConfigNode* n) {
		if (!n) {
			return false;
		}
		if (one && one->accept(n)) {
			return true;
		}
		if (two && two->accept(n)) {
			return true;
		}
		if (three && three->accept(n)) {
			return true;	
		}
		if (four && four->accept(n)) {
			return true;	
		}
		return false;
	}

	bool And::accept(GConfigNode* n) {
		if (!n) {
			return false;
		}
		if (one && !one->accept(n)) {
			return false;
		}
		if (two && !two->accept(n)) {
			return false;
		}
		if (three && !three->accept(n)) {
			return false;	
		}
		if (four && !four->accept(n)) {
			return false;	
		}
		return true;
	}

	bool Type::accept(GConfigNode* n) {
		return n->getTypeId() == type;
	}

	bool Ident::accept(GConfigNode* n) {
		
		string id = simple ? n->getNodeIdent() : n->getFullNodeIdent();
//		cout << id << " vs " << ident << endl;
		return id == ident;
	}

	bool Name::accept(GConfigNode* n) {
		if (!n) return false;
		string nm = n->getNodeName();
//		cerr << "Sammenligner " << nm << " vs " << name << endl;
		return nm == name;
	}
	
	bool Contains::accept(GConfigNode* n) {
		match = false;
		n->visitChildren(this);
		return match;
	}
	
	void Contains::node(GConfigNode* n) {
		if (match) return;
		if (inner->accept(n)) {
			match = true;	
		}
	}
	
	bool ContainedIn::accept(GConfigNode* n) {
		GConfigNode* parent_node = n->getParentNode();
		if (!parent_node) {
			return false;
		}
		if (parent_f->accept(parent_node)) {
			return true;
		}
		switch(parent_node->getTypeId()) {
			case GConfig::STATEMENT_LIST:
			case GConfig::BLOCK_LIST:
				GConfigNode* grandparent_node = parent_node->getParentNode();
				if (grandparent_node) {
					return parent_f->accept(grandparent_node);
				}
				break;
		}
		return false;
	}
	
	bool Value::accept(GConfigNode* n) {
//		cout << "Sjekking if node: " << n << " equals " << value << endl;
		if (n->getTypeId() != GConfig::VARIABLE) {
			return false;	
		}
		GConfigVariableStatement* var = dynamic_cast<GConfigVariableStatement*>(n); 
		if (!var) {
			return false;	
		}
		vector<GConfigScalarVal*> values = var->getValues();

		if (values.size() != 1) {
			return false;
		}
		
		GConfigScalarVal* scalar_val = values[0];
		if (!scalar_val) {
			return false;	
		}
		string str_value = scalar_val->getStringValue();
		if (str_value == value) {
			return true;	
		}

		return false;
	}


} // End namespace filter

void GConfigNodeSearchVisitor::node(GConfigNode* n){
//	cerr << "Level: "<< level << endl;
	if (level == 0) {
//		cerr << "reached level maximum" << endl;
		return;
	}
	if (filter->accept(n)) {
//		cerr << "Match" << endl;
		noder.push_back(n);
	} else {
//		cerr << "Failed" << endl;
	}

	level--;
	n->visitChildren(this);
	level++;
}

GConfigNodeList GConfigNodeList::findNodesByName(const string& name, int type) {
		And navn_og_type(new Name(name), new Type(type));
		GConfigNodeSearchVisitor vst(&navn_og_type);
		for(uint i = 0; i < size(); i++) {
			vst.node(operator[](i));
		}
		return vst.getNodeList();
}

GConfigNodeList GConfigNodeList::findNodesByFilter(filter::NodeFilter* f) {
		GConfigNodeSearchVisitor vst(f);
		for(uint i = 0; i < size(); i++) {
			vst.node(operator[](i));
		}
		return vst.getNodeList();
}


//using namespace dispatch::core::eventfilter;

	vector<string> StringVariableHelper::getStrings(GConfigNode* node, string ident, int mark_used = 0) {
		And find_variables(
			new Ident(ident, true), 
			new Type(GConfig::VARIABLE)
		);
		GConfigNodeList nodes = node->findNodesByFilter(&find_variables);
		GConfigNodeList::iterator i(nodes.begin());
		vector<string> str_vars;
		for(; i != nodes.end();i++) {
			/**
			* Vi kan "trykt" type-caste her, fordi ut fra filter-regelene
			* vi har definert ovenfår _MÅ_ dette være en GConfigVariableStatement.
			* Dette forutsetter selvfølgelig at filtrene fungerer som de skal.
			*/ 
			GConfigVariableStatement* val_stat = dynamic_cast<GConfigVariableStatement*>(*i);
			if (!val_stat) {
				// For sikkerhets skyld
				continue;
			}
			vector<GConfigScalarVal*> values = val_stat->getValues();
			vector<GConfigScalarVal*>::iterator val_i;
			for(val_i = values.begin(); val_i != values.end(); val_i++) {
				GConfigScalarVal* val = (*val_i);
				str_vars.push_back(val->getStringValue());
				if (mark_used) {
					val->used(mark_used);
				}
			}
			if (mark_used) {
				val_stat->used(mark_used);
			}
		}
		return str_vars;

	}
	
	string StringVariableHelper::getString(GConfigNode* node, string ident, int mark_used = 0, bool strict = true) {
		vector<string> strs = StringVariableHelper::getStrings(node, ident, mark_used);
		if (strs.size() != 1) {
			if (strict) {
				// throw
			}
		}
		if (strs.size() > 0) {
			return strs[0];
		}
		return string();
	}

}} // end namespace dispatch::config
