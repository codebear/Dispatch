#include "PHPEventHandler.h"
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace dispatch::config;
using namespace dispatch::config::filter;
namespace dispatch {
namespace module {
namespace phpembed {

/*	PHPEventHandler::PHPEventHandler(PHPEventModule* mod) : filter(NULL) {
		module = mod;
	}*/
	
	PHPEventHandler::PHPEventHandler(PHPEmbedModule* mod, GConfigNode* config) :
		module(mod)
	{
		handler_decl.load_files = StringVariableHelper::getStrings(config, "load_file", 1);
//		vector<string> load_files = StringVariableHelper::getStrings(config, "load_file", 1);
		
//		string klasse = StringVariableHelper::getString(config, "handler_class", 1, true);
		handler_decl.handler_class = StringVariableHelper::getString(config, "handler_class", 1, true);
		
//		string metode = StringVariableHelper::getString(config, "handler_method", 1, true);
		handler_decl.handler_method = StringVariableHelper::getString(config, "handler_method", 1, true);
		
		handler_decl.handler_function = StringVariableHelper::getString(config, "handler_function",.1, true);
//		string funksjon = StringVariableHelper::getString(config, "handler_function", 1, true);
		
		filter = EventFilterHelper::initializeEventFilter(config);
	}
	
	PHPEventHandler::~PHPEventHandler() {
		if (filter) {
			delete filter;
		}
	}
	
	void PHPEventHandler::initializePHPContext(php_stl* php_handler) {
	
		for(vector<string>::iterator i(handler_decl.load_files.begin());i != handler_decl.load_files.end();i++) {
			php_handler->load((*i).c_str());
		}
		
			
		if (handler_decl.handler_class.length() && handler_decl.handler_method.length()) {
			/**
			* Klassenavn deklarert
			*/
			/**
			* Lag en "lambda" funksjon og sett den som handler
			*/
			handler_decl.handler_function = "____secret_handler_lambda____xxx__"+handler_decl.handler_class+"_"+handler_decl.handler_method;;
			php_handler->eval_string("function %s($event) { return %s::%s($event); }", handler_decl.handler_function.c_str(), handler_decl.handler_class.c_str(), handler_decl.handler_method.c_str());
		}
	}
	
		
	bool PHPEventHandler::isValid() {
		if (handler_decl.handler_class.length() && handler_decl.handler_method.length()) {
			return true;
		} else if (handler_decl.handler_function.length()) {
			return true;
		}
		return false;
	}

	void PHPEventHandler::handleEvent(dispatch::Event* e) {
/*		initializePHPContext();
		if (!handler_function.length()) {
			cerr << "Mangler handler function for PHP" << endl;
			return;
		}*/
		if (filter != NULL) {
			/**
			* Skip events som ikke matcher
			*/ 
			module->err() << "Fant filter, sjekker om eventen matcher." << endl;
			if (!filter->match(e)) {
				module->err() << "Event matchet ikke, avbryter." << endl;
				return;
			}
		}
		pid_t child = fork();
		if (child == -1) {
			module->err() << "Failed forking for event handling." << endl;
		} else if (child) {
			/** Parent
			*
			*/
			int status;
			waitpid(child, &status, 0);
		} else {
			/**
			* Child
			*/
			map<string,string> parms = e->getParameters();
			module->err() << "Handterer eventen." << endl;
			_innerHandleEvent(parms);
			cleanup();
			exit(0);
		}
	}
	
	void PHPEventHandler::cleanup() {
		
	}

	void PHPEventHandler::_innerHandleEvent(map<string,string>& parms) {
		php_stl php_handler;
		initializePHPContext(&php_handler);
		map<string,string>::iterator p_iter;
		php_array php_evnt;
		for(p_iter = parms.begin(); p_iter != parms.end(); p_iter++) {
			char* key = strdup((*p_iter).first.c_str());
			char* value = strdup((*p_iter).second.c_str());
			php_evnt.add_assoc("ss", key, value);
			std::free(key);
			std::free(value);
		}
		char* func_name = strdup(handler_decl.handler_function.c_str());
		php_handler.call_void(func_name, "a", &php_evnt);
		std::free(func_name);
	}


}}}
