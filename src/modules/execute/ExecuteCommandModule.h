#ifndef EXECUTECOMMANDMODULE_H_
#define EXECUTECOMMANDMODULE_H_

#include "../DispatchModule.h"
#include "ExecuteCommandHandler.h"

using namespace dispatch::module;
namespace dispatch {
namespace module {
namespace execute {


/**
* Handler som skal kjøre eksekvere et program eller kommando når den mottar en event
*/	
class ExecuteCommandModule : public DispatchModule
{
	vector<ExecuteCommandSpec> commands;
	vector<ExecuteCommandHandler> handlers;
public:
	ExecuteCommandModule();
	virtual ~ExecuteCommandModule();
	
	/**
	* Er ikke en eventkilde
	*/
	virtual bool isEventSource() {
		return false;
	}
	
	/**
	* Er en event handler
	*/
	virtual bool isEventHandler() {
		return true;	
	}
	
	/**
	* Er ikke en service-handler
	*/
	virtual bool isServiceHandler() {
		return false;	
	}
	
	/**
	* Modulnavnet
	*/
	virtual string getModuleName() {
		return string("ExecuteCommand");	
	}
	
	
	/**
	* 1.step initialisering
	*/
	virtual bool preInitialize();
	
	/**
	* Start opp alt
	*/
	virtual bool startup();
	
	/**
	* Steng ned
	*/
	virtual bool shutdown();
	
	/**
	* Motta konfigurasjon
	*/
	virtual bool scanConfigNode(GConfigNode* node);
};


}}}
#endif /*EXECUTE_COMMAND_MODULE_H_*/
