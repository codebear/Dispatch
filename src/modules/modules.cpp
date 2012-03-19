
#include "modules.h"
#include "dlfcn.h"

using namespace std;
using namespace dispatch::config;
using namespace dispatch::config::filter;

namespace dispatch {
namespace module {

_ModuleManagerImpl::_ModuleManagerImpl() 
//: NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) 
{

}

string _ModuleManagerImpl::getName() {
	return "ModuleManager";
}


void _ModuleManagerImpl::loadModules(parseDriver &drv, vector<ModuleEntry> &modules) {
	GConfigNodeList main_blocks = drv.findNodesByName("main", GConfig::BLOCK);

	And load_filter(new Ident("load_modules"), new Type(GConfig::VARIABLE));

	GConfigNodeList load_vars = main_blocks.findNodesByFilter(&load_filter);
	
	/**
	* Merk alle main-blokk-deklareringene som brukt
	*/
	for(uint i = 0; i < main_blocks.size(); i ++) {
		main_blocks[i]->used(1);
	}
//	dbg() << "Vars " << load_vars.size() << endl;
	dbg() << "Loading modules:" << endl;
	
	/**
	* Traverser alle moduler som skal lastes, og se om vi fÃ¥r sparket liv i dem.
	*/
	for(uint i = 0; i < load_vars.size(); ++i) {
		GConfigVariableStatement* var = dynamic_cast<GConfigVariableStatement*>(load_vars[i]);
		vector<GConfigScalarVal*> values = var->getValues();
//		cout << "Variable @ " << var << ": " << values.size() << " verdier" << endl;
		uint c = 0;
		uint j;
		for(j = 0; j < values.size(); ++j) {
			string mod_so = values[j]->getStringValue();
			dbg() << " ** " << mod_so << " **" << endl;
			/**
			*
			*/
			if (loadModule(mod_so, modules)) {
				values[j]->used(1);
			} else {
				++c;
			}
		}
		var->used(1);
	}

	loadModulesConfig(drv, modules);
}

bool _ModuleManagerImpl::loadModule(string mod_file, vector<ModuleEntry> &modules) {

	void* handle = dlopen(mod_file.c_str(), RTLD_NOW | RTLD_LOCAL);

	if (handle == NULL) {
		err() << "Failed loading module " << mod_file << ": " << dlerror() << endl;
		return false;
	}

	ModuleEntry entry;
	
	entry.handle = handle;

	/**
	 * Initalizer
	 */
	void* construct = dlsym(handle, "initialize_dispatch_module");

	if (construct == NULL) {
		err() << "Failed loading constructor from " << mod_file << ": " << dlerror() << endl;
		return false;
	}


	/**
	 * Destructor
	 */
	void* destruct = dlsym(handle, "destroy_dispatch_module");

	if (destruct == NULL) {
		err() << "Failed loading destructor from " << mod_file << ": " << dlerror() << endl;
		return false;
	}

	entry.create = (FUNC_TYPE(DispatchModule*)) (construct);

	entry.destroy = (FUNC_TYPE(void, DispatchModule*)) (destruct);

	/**
	 * Create module-instance
	 */

	DispatchModule* mod = entry.create();

	if (mod == NULL) {
		err() << "Failed creating module-instance from " << mod_file << endl;
		return false;
	}

	err() << "Loaded module: " << mod->getModuleName() << endl;
	entry.module = mod;
	
	modules.push_back(entry);
	return true;
}

void _ModuleManagerImpl::loadModulesConfig(parseDriver &drv, vector<ModuleEntry> &modules) {
	out() << "Leter etter config til moduler" << endl;
	for(uint i = 0; i < modules.size(); i++) {
		DispatchModule* module = modules[i].module;
		string name = module->getModuleName();
		out() << "MODULE: " << name << endl;
		/**
		* Dersom modulen e en eventhandler så må
		* vi gå på leiting etter listener-blokke
		* som har spesifisert den her som module.
		*/
		if (module->isEventHandler()) {
			out() << name << " confirms it's and event handler" << endl;
			// Match alle noder som er en variabel med
			
			And handler_config_filter(
				new Type(GConfig::BLOCK), 
				new Contains(new Contains(new And(
					new Type(GConfig::VARIABLE),
					new Ident("module", true),
					new Value(name)
				))), 
				new Name("handler")
			);
			
			loadConfigNodesIntoModule(drv, module, &handler_config_filter);
		}
		/**
		* Hvis modulen e en event-source så må vi på leiting etter
		* handler-blokke som har den her som module.
		*/
		if (module->isEventSource()) {
			out() << name << " confirms it's an event source" << endl;
			And event_source_config_filter(
				new Type(GConfig::BLOCK), 
				new Contains(new Contains(new And(
					new Type(GConfig::VARIABLE), 
					new Ident("module", true),  
					new Value(name)
				))), 
				new Name("listener")
			);
			
			loadConfigNodesIntoModule(drv, module, &event_source_config_filter);
		}
	}
	out() << "Done." << endl;
}


/**
* Denne metoden fÃ¥r 
*  - en parse-driver, som inneholder hele konfigurasjonen (alle filene)
*  - module-peker som peker pÃ¥ modulen som vi skal dytte konfigurasjonen inn i 
*  - node-filter som brukes for Ã¥ hente ut blokkene som denne modulen skal ta hÃ¥nd om.
*/
void _ModuleManagerImpl::loadConfigNodesIntoModule(parseDriver &drv, DispatchModule* module, NodeFilter* filter) {
		string name = module->getModuleName();

		GConfigNodeList nodes = drv.findNodesByFilter(filter);

		/**
		* Filter for Ã¥ hente ut deklarasjonen 
		* module = Foo
		*/
		And module_var(
			new Type(GConfig::VARIABLE),
			new Ident("module", true)
		);

		for(uint j = 0; j < nodes.size(); j++) {
			GConfigBlock* block = dynamic_cast<GConfigBlock*>(nodes[j]);
//			out << "MARK1" << endl;
//			out << "Found node. Name: " << block->getNodeName() << " ident: " << block->getFullNodeIdent() << endl;
			bool res = false;
			try {
				res = module->scanConfigNode(block);	
			} catch (GConfigParseError ex) {
				stringstream ss;
				ss <<"Parse error funnet: " << ex.getMessage() << endl;
				err() << ss.str();
				block->error(ss.str());
			}
//			out << "MARK2" << endl;
			if (res) {
//				out << "Module mottok konfig med glede\n" << endl;
				/**
				* Let opp module= deklarasjonen og merk den som brukt.
				*/
				GConfigNodeList mod_vars = block->getStatementList()->findNodesByFilter(&module_var);
				GConfigNodeList::iterator mod_var_it;
				for (mod_var_it = mod_vars.begin(); mod_var_it != mod_vars.end(); mod_var_it++) {
					out() << "Merker module=Foo node som brukt: " << (*mod_var_it) << endl;
					(*mod_var_it)->used(1);
				}
				/**
				* Denne neste greie med Ã¥ merke alt virker noe forskrudd
				*/
//				out() << "Merker hele config-blokken som i bruk??" << endl;
//				block->used(1);


//			} else {
//				out << "Module ville ikke ha konfig" << endl;
			}
//			out << "MARK3" << endl;
		}
	
}

void _ModuleManagerImpl::unloadModule(ModuleEntry& mod) {
	dlclose(mod.handle);
}

}} // end namespace
