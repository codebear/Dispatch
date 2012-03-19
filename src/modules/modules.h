
#ifndef _MODULES_H
#define _MODULES_H 1
#include "DispatchModule.h"
#include "../config/driver.h"
#include "../config/config_nodes.h"
#include "../config/node_list.h"
#include "../util/Singleton.h"
#include <vector>
using namespace std;
using namespace dispatch::config;

#define FUNC_PTR(ret, name, ...) ret(*name)(__VA_ARGS__)
#define FUNC_TYPE(ret, ...) ret (*)(__VA_ARGS__)

namespace dispatch {
namespace module {
	typedef unsigned int uint;

	/**
	* Innslag i master-modul-tabellen til systemet
	*/
	class ModuleEntry {
	public:
		/**
		* DL-handle som returnert fra dlopen
		*/
		void* handle;
		/**
		* Peker til instans av modulen
		*/
		DispatchModule* module;
		
		/**
		* Peker til metoden som lastes fra en .so fil som instansiere en instans av modulen
		*/
		FUNC_PTR(DispatchModule*, create);
		
		/**
		* Peker til metoden som lastes fra en .so fil som frigjør en instans av modulen
		*/
		FUNC_PTR(void, destroy, DispatchModule*);
	};

	/**
	* Visitor for å lese konfigurasjon for moduler
	* @todo Dokumenter bedre hvordan denne virker
	*/
	class ModuleConfigVisitor : public GConfigNodeVisitor {
		public:
			/**
			* @todo Få klarhet i hvordan denne brukes
			*/
			void node(GConfigNode* n);
			
			/**
			* @todo Få klarhet i hvordan denne brukes
			*/
			void module(GConfigBlock* n);
	};

	/**
	* Støtteklasse som tar seg av det med lasting av moduler via shared-objekt-filer
	*/
	class _ModuleManagerImpl : public NameTimeTaggingOutputSet {
	public:
		_ModuleManagerImpl();
		
		/**
		* Navn for output-set
		*/
		string getName();
		/**
		* Last moduler ihht til konfigurasjonsfilen
		*/
		void loadModules(parseDriver &drv, vector<ModuleEntry> &modules);
		
		/**
		* Load s module from file and insert into module-vector
		*/
		bool loadModule(string filename, vector<ModuleEntry> &modules);
		
		/**
		* Overfør konfigurasjon til moduler
		*/
		void loadModulesConfig(parseDriver &drv, vector<ModuleEntry> &modules);

		/**
		* Load Config nodes into a single module
		*/
		void loadConfigNodesIntoModule(parseDriver &drv, DispatchModule* module, filter::NodeFilter* filter);

		/**
		* Unload shared object-file loaded
		*/
		void unloadModule(ModuleEntry& mod);

	};
	
	typedef Singleton<_ModuleManagerImpl> ModuleManager;
}} // end namespace

#endif
