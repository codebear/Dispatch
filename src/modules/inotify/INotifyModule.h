#ifndef INOTIFYMODULE_H_
#define INOTIFYMODULE_H_

#include "../DispatchModule.h"
#include "INotifyListener.h"

using namespace dispatch::config;
using namespace dispatch::config::filter;

namespace dispatch {
namespace module {
namespace inotify {
	
/**
* Modul som generer eventer ved overvåkning av kataloger ved hjelp av inotify.
*/
class INotifyModule : public DispatchModule
{
	/**
	* Foreløpig bare støtte for å overvåke en mappe, dermed en handler
	*/
	INotifyListener handler;
public:
	INotifyModule();
	virtual ~INotifyModule();
	
	/**
	* Dette er en event kilde
	*/
	virtual bool isEventSource() {
		return true;
	}
	
	/**
	* Dette er ikke en event handler
	*/
	virtual bool isEventHandler() {
		return false;
	}
	
	/**
	* Dette er ikke en service-handler
	*/
	virtual bool isServiceHandler() {
		return false;	
	}
	
	/**
	* Modulnavn
	*/
	virtual string getModuleName() {
		return string("INotify");	
	}
	
	/**
	* 1.step initialisering av modulen
	*/
	virtual bool preInitialize();
	
	/**
	* Start opp overvåkning
	*/
	virtual bool startup();
	
	/**
	* Steng ned
	*/
	virtual bool shutdown();
	
	/**
	* Ta imot konfigurasjon
	*/
	virtual bool scanConfigNode(GConfigNode* node);
};

}}}
#endif /*PHPEMBEDMODULE_H_*/
