#include "PHPEventHandler.h"
#include <map>

using namespace dispatch::config;
using namespace dispatch::config::filter;
namespace dispatch {
namespace module {
namespace phpembed {

	PHPEventHandler::PHPEventHandler() : filter(NULL) {
	
	}
	
	PHPEventHandler::PHPEventHandler(GConfigNode* config) {
		cout << "fikk en konfig node her" << endl;
		
		vector<string> load_files = StringVariableHelper::getStrings(config, "load_file", 1);
		
		string klasse = StringVariableHelper::getString(config, "handler_class", 1, true);
		
		string metode = StringVariableHelper::getString(config, "handler_method", 1, true);
		
		string funksjon = StringVariableHelper::getString(config, "handler_function", 1, true);
		
		for(vector<string>::iterator i(load_files.begin());i != load_files.end();i++) {
			php_handler.load((*i).c_str());
		}
		
		filter = EventFilterHelper::initializeEventFilter(config);
		
		if (klasse.length() && metode.length()) {
			/**
			* Klassenavn deklarert
			*/
			/**
			* Lag en "lambda" funksjon og sett den som handler
			*/
			handler_function = "____secret_handler_lambda____xxx__";
			php_handler.eval_string("function %s($event) { return %s::%s($event); }", handler_function.c_str(), klasse.c_str(), metode.c_str());
			
		} else if (funksjon.length()) {
			/**
			* Funksjon deklarert
			*/
			handler_function = funksjon;
		}
	}
	
	PHPEventHandler::~PHPEventHandler() {
		if (filter) {
			delete filter;
		}
	}
	
		
	bool PHPEventHandler::isValid() {
		return true;
	}

	void PHPEventHandler::handleEvent(dispatch::Event* e) {
		if (!handler_function.length()) {
			cerr << "Mangler handler function for PHP" << endl;
			return;
		}
		if (filter != NULL) {
			/**
			* Skip events som ikke matcher
			*/ 
			cerr << "Fant filter, sjekker om eventen matcher." << endl;
			if (!filter->match(e)) {
				cerr << "Event matchet ikke, avbryter." << endl;
				return;
			}
		}
		map<string,string> parms = e->getParameters();
		cerr << "Handterer eventen." << endl;
		_innerHandleMap(parms);
	}

	void PHPEventHandler::_innerHandleMap(map<string,string>& parms) {	
		map<string,string>::iterator p_iter;
		php_array php_evnt;
		for(p_iter = parms.begin(); p_iter != parms.end(); p_iter++) {	
			char* key = strdup((*p_iter).first.c_str());
			char* value = strdup((*p_iter).second.c_str());
			php_evnt.add_assoc("ss", key, value);
			std::free(key);
			std::free(value);
		}
		char* func_name = strdup(handler_function.c_str());
		php_handler.call_void(func_name, "a", &php_evnt);
		std::free(func_name);
	}


}}}
