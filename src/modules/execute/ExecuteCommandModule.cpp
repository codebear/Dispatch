
#include "ExecuteCommandModule.h"
#include "../../config/node_list.h"

using namespace dispatch::config;


using namespace dispatch::module;
namespace dispatch {
namespace module {
namespace execute {

ExecuteCommandModule::ExecuteCommandModule()
{
}

ExecuteCommandModule::~ExecuteCommandModule()
{
}

bool ExecuteCommandModule::preInitialize() {
	return true;
}

bool ExecuteCommandModule::startup() {
	vector<ExecuteCommandSpec>::iterator h_it;
	for(h_it = commands.begin(); h_it < commands.end(); h_it++) {
		ExecuteCommandHandler h(*h_it);
		handlers.push_back(h);
	}
	return true;
}

bool ExecuteCommandModule::shutdown() {
	return true;
}

bool ExecuteCommandModule::scanConfigNode(GConfigNode* node) {
//		out << "PHPEmbedModule:: Fikk en configNode: " << node << endl;
/*
		PHPEventHandler* handler = new PHPEventHandler(node);
		if (handler->isValid()) {
			handlers.push_back(handler);
			getEventQueue()->registerHandler(handler);
			return true;
		}
		delete handler;
*/

	string cmd = StringVariableHelper::getString(node, "command", 1, true);
	
	string event_passing = StringVariableHelper::getString(node, "event_passing", 1, true);
	
	cout <<
		"ExecuteCommandModule: " <<
		" command: " << cmd << 
		" event_passing: " << event_passing <<
		endl;
	
	ExecuteCommandSpec spec(cmd, event_passing);
	commands.push_back(spec);
		
/*
	string metode = StringVariableHelper::getString(config, "handler_method", 1, true);

	string funksjon = StringVariableHelper::getString(config, "handler_function", 1, true);

	cout << "ExecuteCommandModule: Bedt om å scan en config-node:" << node << endl;
	*/
	return true;
}

}}} // end namespace dispatch::module::execute

extern "C" {

	extern dispatch::module::DispatchModule* initialize_dispatch_module() {
		return new dispatch::module::execute::ExecuteCommandModule();
	}

	extern void destroy_dispatch_module(dispatch::module::DispatchModule* ptr) {
		delete ptr;
	}

}
