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
	* Single instans event-handler for kj�ring av PHP
	*/
	class PHPEventHandler : public EventHandler {
		PHPEmbedModule* module;
		/**
		* PHPEmbed handle
		*/
//		php_stl php_handler;
		
		PHPEventHandlerDeclaration handler_decl;
		
		/**
		* Navn p� funksjonen som skal kj�res. Gjelder ogs� med klasse::metode kall. (da inneholder denne navnet p� en wrapper-funksjon som s� kaller statisk inn i klassen)
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
		* Tar en konfig-node som inneholder konfigurasjonen for hvordan denne skal oppf�re seg
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
		* H�ndter en event (EventHandler-interfacet)
		*/
		void handleEvent(dispatch::Event* e);
		
		/**
		* Cleanup before shutdown
		*/
		void cleanup();
	};
}}}
#endif

