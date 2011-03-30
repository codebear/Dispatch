
#include "modules.h"
#include "dlfcn.h"

using namespace std;
using namespace dispatch::config;
using namespace dispatch::config::filter;
namespace dispatch {
namespace module {

ModuleManager::ModuleManager() : 
	NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) {

}

string ModuleManager::getName() {
	return "ModuleManager";
}


void ModuleManager::loadModules(parseDriver &drv, vector<ModuleEntry> &modules) {
	GConfigNodeList mod_blocks = drv.findNodesByName("main", GConfig::BLOCK);

	And load_filter(new Ident("load_modules"), new Type(GConfig::VARIABLE));

	GConfigNodeList load_vars = mod_blocks.findNodesByFilter(&load_filter);
	
	for(int i = 0; i < mod_blocks.size(); i ++) {
		mod_blocks[i]->used(1);	
	}
	err << "Vars " << load_vars.size() << endl;
	err << "Loading modules:" << endl;
	
	for(int i = 0; i < load_vars.size(); i++) {
		GConfigVariableStatement* var = dynamic_cast<GConfigVariableStatement*>(load_vars[i]);
		vector<GConfigScalarVal*> values = var->getValues();
//		cout << "Variable @ " << var << ": " << values.size() << " verdier" << endl;
		for(int j = 0; j < values.size(); j++) {
			string mod_so = values[j]->getStringValue();
			err << " ** " << mod_so << " **" << endl;
			loadModule(mod_so, modules);
			values[j]->used(1);
		}
		var->used(1);
	}

	loadModulesConfig(drv, modules);
}

void ModuleManager::loadModule(string mod_file, vector<ModuleEntry> &modules) {

	void* handle = dlopen(mod_file.c_str(), RTLD_NOW | RTLD_LOCAL);

	if (handle == NULL) {
		err << "Failed loading module " << mod_file << ": " << dlerror() << endl;
		return;
	} else {

	}

	ModuleEntry entry;

	/**
	 * Initalizer
	 */
	void* func = dlsym(handle, "initialize_dispatch_module");

	if (func == NULL) {
		err << "Failed loading constructor from " << mod_file << ": " << dlerror() << endl;
		return;
	}

	entry.create = (FUNC_TYPE(DispatchModule*)) (func);

	/**
	 * Destructor
	 */
	func = dlsym(handle, "destroy_dispatch_module");

	if (func == NULL) {
		err << "Failed loading destructor from " << mod_file << ": " << dlerror() << endl;
		return;
	}

	entry.destroy = (FUNC_TYPE(void, DispatchModule*)) (func);

	/**
	 * Create module-instance
	 */

	DispatchModule* mod = entry.create();

	if (mod == NULL) {
		err << "Failed creating module-instance from " << mod_file << endl;
	}

	err << "Loaded module: " << mod->getModuleName() << endl;
	entry.module = mod;
	modules.push_back(entry);

}

void ModuleManager::loadModulesConfig(parseDriver &drv, vector<ModuleEntry> &modules) {
	out << "Leter etter config til moduler" << endl;
	for(int i = 0; i < modules.size(); i++) {
		DispatchModule* module = modules[i].module;
		string name = module->getModuleName();
		out << "MODULE: " << name << endl;
		/**
		* Dersom modulen e en eventhandler så må
		* vi gå på leiting etter listener-blokke
		* som har spesifisert den her som module.
		*/
		if (module->isEventHandler()) {
			out << name << " confirms it's and event handler" << endl;
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
			out << name << " confirms it's an event source" << endl;
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
	out << "Done." << endl;
}

void ModuleManager::loadConfigNodesIntoModule(parseDriver &drv, DispatchModule* module, NodeFilter* filter) {
		string name = module->getModuleName();

		And module_var(
			new Type(GConfig::VARIABLE),
			new Ident("module", true)
		);
			
		GConfigNodeList nodes = drv.findNodesByFilter(filter);
		for(int j = 0; j < nodes.size(); j++) {
			GConfigBlock* block = dynamic_cast<GConfigBlock*>(nodes[j]);
//			out << "MARK1" << endl;
//			out << "Found node. Name: " << block->getNodeName() << " ident: " << block->getFullNodeIdent() << endl;
			bool res = false;
			try {
				res = module->scanConfigNode(block);	
			} catch (GConfigParseError ex) {
				err << "Parse error funnet: " << ex.getMessage() << endl;
			}
//			out << "MARK2" << endl;
			if (res) {
//				out << "Module mottok konfig med glede\n" << endl;
				GConfigNodeList mod_vars = block->getStatementList()->findNodesByFilter(&module_var);
				GConfigNodeList::iterator mod_var_it;
				for (mod_var_it = mod_vars.begin(); mod_var_it != mod_vars.end(); mod_var_it++) {
//					out << "Merker node som brukt: " << (*mod_var_it) << endl;
					(*mod_var_it)->used(1);
				}
				block->used(1);
//			} else {
//				out << "Module ville ikke ha konfig" << endl;
			}
//			out << "MARK3" << endl;
		}
	
}

}} // end namespace
