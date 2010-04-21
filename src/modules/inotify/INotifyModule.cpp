#include "INotifyModule.h"
#include <vector>

using namespace dispatch::config;
using namespace dispatch::config::filter;
namespace dispatch {
namespace module {
namespace inotify {
	INotifyModule::INotifyModule() {}
	INotifyModule::~INotifyModule() {}
	bool INotifyModule::preInitialize() {
		return true;
	}
	
	bool INotifyModule::startup() {
		err << "Starting up inotify listener" << endl;
		handler.start();
		return handler.isRunning();
	}
	
	bool INotifyModule::shutdown() {
		handler.stop();
		return !handler.isStopped();
	}
	
	bool INotifyModule::scanConfigNode(GConfigNode* node) {
		vector<string> paths = StringVariableHelper::getStrings(node, "watch", 1);
		vector<string>::iterator p_iter;
		for(p_iter = paths.begin(); p_iter != paths.end(); p_iter++) {
			err << "Fant sti som skal overvåkes av inotify: " << *p_iter << endl;
			handler.addWatch(*p_iter);
		}
		return true;
	}

}}}

extern "C" {

	extern dispatch::module::DispatchModule* initialize_dispatch_module() {
		return new dispatch::module::inotify::INotifyModule();
	}

	extern void destroy_dispatch_module(dispatch::module::DispatchModule* ptr) {
		if (ptr != NULL) {
			delete ptr;
		}
	}

}
