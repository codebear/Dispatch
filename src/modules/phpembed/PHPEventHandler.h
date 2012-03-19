#ifndef _PHP_EVENT_HANDLER_H
#define _PHP_EVENT_HANDLER_H 1

#include <php_stl.h>
#include "php_compat_fix.h"

#include "../DispatchModule.h"
#include "../../core/EventQueue.h"
#include "../../core/eventfilter/EventFilter.h"
#include "../../core/eventfilter/EventFilterHelper.h"
#include "PHPEmbedModule.h"


using namespace dispatch::config;
using namespace dispatch::core::eventfilter;

namespace dispatch {
namespace module {
namespace phpembed {
	
	class PHPEventHandlerDeclaration {
	public:
		string handler_function;
		string handler_class;
		string handler_method;
		vector<string> load_files;
	};
	
	/**
	* Single instans event-handler for kjøring av PHP
	*/
	class PHPEventHandler : public EventHandler {
		PHPEmbedModule* module;
		/**
		* PHPEmbed handle
		*/
//		php_stl php_handler;
		
		PHPEventHandlerDeclaration handler_decl;
		
		/**
		* Navn på funksjonen som skal kjøres. Gjelder også med klasse::metode kall. (da inneholder denne navnet på en wrapper-funksjon som så kaller statisk inn i klassen)
		*/
//		string handler_function;
		
		void initializePHPContext(php_stl* h);
		/**
		* Filtrer eventer med dette filteret
		*/
		EventFilter* filter;
		
		void _innerHandleEvent(map<string, string>&parms);
		
//		void _forkHandler();
	public:

//		PHPEventHandler(PHPEventModule* mod);
		/**
		* Tar en konfig-node som inneholder konfigurasjonen for hvordan denne skal oppføre seg
		*/
		PHPEventHandler(PHPEmbedModule*, GConfigNode* config);
		
		/**
		* Destructor
		*/
		virtual ~PHPEventHandler();
		
		/**
		* Er alt iorden
		*/
		bool isValid();
		
		/**
		* Håndter en event (EventHandler-interfacet)
		*/
		void handleEvent(dispatch::Event* e);
		
		/**
		* Cleanup before shutdown
		*/
		void cleanup();
	};
}}}
#endif

