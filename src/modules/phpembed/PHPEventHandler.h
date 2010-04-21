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
	* Single instans event-handler for kjøring av PHP
	*/
	class PHPEventHandler : public EventHandler {
		/**
		* PHPEmbed handle
		*/
		php_stl php_handler;
		
		/**
		* Navn på funksjonen som skal kjøres. Gjelder også med klasse::metode kall. (da inneholder denne navnet på en wrapper-funksjon som så kaller statisk inn i klassen)
		*/
		string handler_function;
		
		void _innerHandleMap(map<string, string>&parms);
	public:

		PHPEventHandler();
		/**
		* Tar en konfig-node som inneholder konfigurasjonen for hvordan denne skal oppføre seg
		*/
		PHPEventHandler(GConfigNode* config);
		
		/**
		* Er alt iorden
		*/
		bool isValid();
		
		/**
		* Håndter en event (EventHandler-interfacet)
		*/
		void handleEvent(dispatch::Event* e);
	};
}}}
#endif

