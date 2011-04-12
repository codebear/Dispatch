#include "IPCMsgModule.h"
#include "SysVMsgQueue.h"
#include "../../config/node_list.h"
#include "../../util/StringHelper.h"

using namespace dispatch::config::filter;
namespace dispatch { namespace module { namespace ipcmsg {

_ListenerSpec::_ListenerSpec(key_t k, int t, int c, NodeIdent id) :
	key(k),
	type(t),
	count(c),
	ident(id) {


}

//IPCMsgModule::IPCMsgModule();

string IPCMsgModule::getModuleName() {
	return "IPCMsgSource";
}

bool IPCMsgModule::isEventSource() {
	return true;
}

bool IPCMsgModule::isEventHandler() {
	return false;
}


bool IPCMsgModule::shutdown() {
	vector<IPCMsgListener*>::iterator l_it;
	bool retval = false;
	for(l_it = listeners.begin(); l_it != listeners.end(); l_it++) {
		(*l_it)->stop();
		retval = true;
	}
	return retval;
}

bool IPCMsgModule::startup() {
	vector<IPCMsgListener*>::iterator l_it;
	bool retval = false;
	for(l_it = listeners.begin(); l_it != listeners.end(); l_it++) {
		out << "Starter opp ipc-lytter " << endl;
		(*l_it)->start();
		retval = true;
	}
	return retval;
}

bool IPCMsgModule::preInitialize() {
	vector<_ListenerSpec*>::iterator it;
	for(it = specs.begin(); it < specs.end(); it++) {
		initializeListeners((*it)->key, (*it)->type, (*it)->count, (*it)->ident);
	}
	return true;
}


bool IPCMsgModule::scanConfigNode(GConfigNode* node) {
	vector<string> keys = StringVariableHelper::getStrings(node, "key", 1);
	vector<string> types = StringVariableHelper::getStrings(node, "type", 1);
	string count_str = StringVariableHelper::getString(node, "threads", 1, true);
	NodeIdent ident = node->getFullNodeIdent();
	int count = 1;
	if (count_str.length()) {
		count = string_conv<int>(count_str);
		if (count < 1) {
			err << "Thread count was less than 1, setting it to 1" << endl;
			count = 1;
		}
	}
	vector<string>::iterator key_it;
	vector<string>::iterator type_it;
	bool retval = false;
	for(key_it = keys.begin(); key_it != keys.end(); key_it++) {
		int key = string_conv<int>(*key_it);
		if (!key) {
			err << "Key " << *key_it << " is illegal" << endl;
			continue;
		}
		
		if (types.size()) {
			for(type_it = types.begin(); type_it != types.end(); type_it++) {
				long type = string_conv<long>(*type_it);
				if (type) {
					specs.push_back(new _ListenerSpec(key, type, count, ident));
//					initializeListeners(key, type, count);
					retval = true;
				} else {
					err << "Type " << *type_it << " is illegal" << endl;
				}
				
			}
		} else {
			specs.push_back(new _ListenerSpec(key, 0, count, ident));
	//		initializeListeners(key, 0, count);
			retval = true;
		}
	}
	return retval;
}


void IPCMsgModule::initializeListeners(key_t key, long type, int count, NodeIdent ident) {
	out << "Initaliserer " << count << " ipc-lyttere med key " << key << " og type " << type << endl;
	for(int i = 0; i < count; i++) {
		if (type) {
			listeners.push_back(new IPCMsgListener(ident, new SysVMsgQueue<IPCMsg>(key), getEventQueue()));
		} else {
			listeners.push_back(new IPCMsgListener(ident, new SysVMsgQueue<IPCMsg>(key), getEventQueue(), type));		
		}
	}
}



}}} // end namespace

extern "C" {

	extern dispatch::module::DispatchModule* initialize_dispatch_module() {
		return new dispatch::module::ipcmsg::IPCMsgModule();
	}

	extern void destroy_dispatch_module(dispatch::module::DispatchModule* ptr) {
		delete ptr;
	}

}

