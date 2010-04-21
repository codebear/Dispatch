#ifndef EXECUTECOMMANDHANDLER_H_
#define EXECUTECOMMANDHANDLER_H_

#include "../DispatchModule.h"

using namespace dispatch::module;
namespace dispatch {
namespace module {
namespace execute {

/**
* Handler som skal kjøre eksekvere et program eller kommando når den mottar en event
*/	
class ExecuteCommandHandler : public DispatchModule
{
public:
	ExecuteCommandHandler();
	virtual ~ExecuteCommandHandler();
	
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
#endif /*EXECUTECOMMANDHANDLER_H_*/
