
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

	string cmd = StringVariableHelper::getString(node, "command", 1, true);
	
	string event_passing = StringVariableHelper::getString(node, "event_passing", 1, true);
	
	out() <<
		"ExecuteCommandModule: " <<
		" command: " << cmd << 
		" event_passing: " << event_passing <<
		endl;
	
	ExecuteCommandSpec spec(cmd, event_passing);
	commands.push_back(spec);
		
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
