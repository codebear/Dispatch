

#include "ExecuteCommandHandler.h"

namespace dispatch { namespace module { namespace execute {

ExecuteCommandSpec::ExecuteCommandSpec(string c, string ep) {
	command = c;
	if (ep == "pipe") {
		event_passing = PIPE;
	} else if(ep == "argv") {
		event_passing = ARGV;
	} else if (ep == "ENV") {
		event_passing = ENV;
	} else {
		event_passing = NONE;
	}
}

ExecuteCommandSpec::ExecuteCommandSpec(string c, event_pass_t ep) {
	command = c;
	event_passing = ep;
}


ExecuteCommandHandler::ExecuteCommandHandler(ExecuteCommandSpec spec) {

}

}}} // end namespace dispatch.module.execute