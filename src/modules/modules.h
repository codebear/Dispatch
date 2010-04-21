
#ifndef _MODULES_H
#define _MODULES_H 1
#include "DispatchModule.h"
#include "../config/driver.h"
#include "../config/config_nodes.h"
#include <vector>
using namespace std;
using namespace dispatch::config;

#define FUNC_PTR(ret, name, ...) ret(*name)(__VA_ARGS__)
#define FUNC_TYPE(ret, ...) ret (*)(__VA_ARGS__)

namespace dispatch {
namespace module {

	/**
	* Innslag i master-modul-tabellen til systemet
	*/
	class ModuleEntry {
	public:
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
	class ModuleManager : public NameTimeTaggingOutputSet {
	public:
		ModuleManager();
		
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
		void loadModule(string filename, vector<ModuleEntry> &modules);
		
		/**
		* Overfør konfigurasjon til moduler
		*/
		void loadModulesConfig(parseDriver &drv, vector<ModuleEntry> &modules);
	};
}} // end namespace

#endif
