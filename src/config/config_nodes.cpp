#include "config_nodes.h"
namespace dispatch {
namespace config {

		GConfigArgumentList::~GConfigArgumentList() {
			for(uint i = 0; i < arguments.size(); i++) {
				GConfigScalarVal* v = arguments[i];
				if (v) {
					delete v;
				}
			}
			arguments.clear();
		}

		GConfigBlock::~GConfigBlock() {
			if (_head) {
				delete _head;
			}
			if (_list) {
				delete _list;
			}
		}


		std::ostream& GConfigBlock::operator<<(std::ostream& os) {
			os << "GConfigBlock {\n";
			os << *_head;
			os << ";";
			os << *_list;
			os << "}\n";
			return os;
		}
		
		
	/**
	 * GConfigVariableStatement
	 */


	string GConfigVariableStatement::getNodeIdent() {
		if (vname) {
			return vname->getNodeIdent();
		}
		return string();
	}

}}
