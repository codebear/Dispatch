#ifndef _EXECUTE_COMMAND_HANDLER_H
#define _EXECUTE_COMMAND_HANDLER_H

#include <string>

using namespace std;

namespace dispatch { namespace module { namespace execute {

enum event_pass_t {
	NONE = 0,
	PIPE = 1,
	ARGV = 2,
	ENV = 3
};

class ExecuteCommandSpec {
public:
	string command;
	event_pass_t event_passing;
	
	ExecuteCommandSpec(string c, string ep);
	ExecuteCommandSpec(string c, event_pass_t ep);
};


class ExecuteCommandHandler {
public:
	ExecuteCommandHandler(ExecuteCommandSpec s);
};

}}} // end namespace dispatch.module.execute

#endif