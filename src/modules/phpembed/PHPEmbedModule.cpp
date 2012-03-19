#include "PHPEmbedModule.h"
#include "PHPEventHandler.h"
#include <vector>

using namespace dispatch::config;
using namespace dispatch::config::filter;

namespace dispatch {
namespace module {
namespace phpembed {
	vector<PHPEventHandler*> handlers;
	PHPEmbedModule::PHPEmbedModule() {

	}

	PHPEmbedModule::~PHPEmbedModule() {
	
	}


	bool PHPEmbedModule::preInitialize() {
		vector<PHPEventHandler*>::iterator handler;
		for(handler = handlers.begin(); handler < handlers.end(); handler++) {
			getEventQueue()->registerHandler(*handler);
		}
		return true;
	}

	bool PHPEmbedModule::startup() {
		return true;
	}

	bool PHPEmbedModule::shutdown() {
		vector<PHPEventHandler*>::iterator handle_i;
		for(handle_i = handlers.begin(); handle_i != handlers.end(); handle_i++) {
			delete *handle_i;
		}
	
		return true;
	}
	
	bool PHPEmbedModule::scanConfigNode(GConfigNode* node) {
//		out << "PHPEmbedModule:: Fikk en configNode: " << node << endl;
		PHPEventHandler* handler = new PHPEventHandler(this, node);
		if (handler->isValid()) {
			handlers.push_back(handler);
//			getEventQueue()->registerHandler(handler);
			return true;
		}
		delete handler;
		return false;
	}
}}}

extern "C" {

	extern dispatch::module::DispatchModule* initialize_dispatch_module() {
		return new dispatch::module::phpembed::PHPEmbedModule();
	}

	extern void destroy_dispatch_module(dispatch::module::DispatchModule* ptr) {
		delete ptr;
	}

}
