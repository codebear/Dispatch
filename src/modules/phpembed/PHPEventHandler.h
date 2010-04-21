#ifndef _PHP_EVENT_HANDLER_H
#define _PHP_EVENT_HANDLER_H 1

#include "../DispatchModule.h"
#include "../../core/EventQueue.h"
#include <php_stl.h>
#include "php_compat_fix.h"


using namespace dispatch::config;

namespace dispatch {
namespace module {
namespace phpembed {
	
	/**
	* Single instans event-handler for kj�ring av PHP
	*/
	class PHPEventHandler : public EventHandler {
		/**
		* PHPEmbed handle
		*/
		php_stl php_handler;
		
		/**
		* Navn p� funksjonen som skal kj�res. Gjelder ogs� med klasse::metode kall. (da inneholder denne navnet p� en wrapper-funksjon som s� kaller statisk inn i klassen)
		*/
		string handler_function;
		
		void _innerHandleMap(map<string, string>&parms);
	public:

		PHPEventHandler();
		/**
		* Tar en konfig-node som inneholder konfigurasjonen for hvordan denne skal oppf�re seg
		*/
		PHPEventHandler(GConfigNode* config);
		
		/**
		* Er alt iorden
		*/
		bool isValid();
		
		/**
		* H�ndter en event (EventHandler-interfacet)
		*/
		void handleEvent(dispatch::Event* e);
	};
}}}
#endif

