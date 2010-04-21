#include "ExecuteCommandHandler.h"
using namespace dispatch::module;
namespace dispatch {
namespace module {
namespace execute {

ExecuteCommandHandler::ExecuteCommandHandler()
{
}

ExecuteCommandHandler::~ExecuteCommandHandler()
{
}

bool ExecuteCommandHandler::preInitialize() {
	return true;
}

bool ExecuteCommandHandler::startup() {
	return false;
}

bool ExecuteCommandHandler::shutdown() {
	return false;
}

bool ExecuteCommandHandler::scanConfigNode(GConfigNode* node) {
	cout << "ExecuteCommandHandler: Bedt om å scan en config-node:" << node << endl;
	return true;
}

}}}

extern "C" {

	extern dispatch::module::DispatchModule* initialize_dispatch_module() {
		return new dispatch::module::execute::ExecuteCommandHandler();
	}

	extern void destroy_dispatch_module(dispatch::module::DispatchModule* ptr) {
		delete ptr;
	}

}
