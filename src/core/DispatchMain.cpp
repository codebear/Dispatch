
#include "../config/config.h"
#include "EventQueue.h"

#include <string>
#include <vector>
#include "../modules/DispatchModule.h"
#include "../modules/modules.h"
#include "DispatchMain.h"
#include "../util/Functor.h"
#include "../util/FDMonitor.h"

using namespace std;
using namespace dispatch;
using namespace dispatch::module;
using namespace dispatch::util;

namespace dispatch { namespace core {

DispatchMain::DispatchMain(int argc, char* argv[]) : 
	NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) ,
	queue(0),
	dont_detach(false)
{
	for(int i = 0; i < argc; i++) {
		string str = argv[i];
		ARGV.push_back(str); //ing(argv[i]));
	}
}

DispatchMain::~DispatchMain() {

}

string DispatchMain::getName() {
	return "DispatchMain";
}

EventQueue* DispatchMain::getEventQueue() {
	if (!queue) {
		queue = new ThreadedEventQueue();
	}
	return queue;
}

void DispatchMain::initialize() {
	SigHandler::instance()->registerListener(SIGTERM, this);
	SigHandler::instance()->registerListener(SIGINT, this);
}


void DispatchMain::handleSignal(int sig) {
	err << "DISPATCH: Received signal " << sig << " " << SigHandler::signalName(sig) << endl;
	switch(sig) {
		case SIGINT:
		case SIGTERM:
			break;
	}
}

void DispatchMain::cleanup() {

}

int DispatchMain::start() {
	initialize();
	int ret = run();
	cleanup();
	return ret;
}
/*
class A {
public:
	void doThing(bool (*isRunning)()) {
		
		int cnt = 0;
		while(isRunning()) {
			Thread* tptr = Thread::currentThread();
			cerr << "Thread magic: "<< cnt++ << endl;
			if (tptr == NULL) {
				cerr << "Current thread is not available" << endl;
			} else {
				cerr << "Is in a different thread. Running:" << tptr->isRunning() << endl;
			}
			sleep(1);
		}
		cerr << "Thread har avsluttet" << endl;
	}
};
*/	
string DispatchMain::getConfigFile() {
	return config_file;
}

string DispatchMain::getConfigDir() {
	return config_dir;
}

void DispatchMain::cmdLineHelp() {
	cout << "usage:" << endl;
	cout << "	dispatch [options]" << endl;
	cout << "options: " << endl;
	cout << "	-h"<< endl;
	cout << "	--help" << endl;
	cout << "		Show this help-screen" << endl;
	cout << "	-c <base_config_file>" << endl;
	cout << "	--config <base_config_file>" << endl;
	cout << "		Load <base_config_file> as det initial configuration" << endl;
	cout << "	-d <base_config_dir>" << endl;
	cout << "	--config-dir <base_config_dir>" << endl;
	cout << "		Set <base_config_dir> as the relative base-path for file-references in config-files" << endl;
	cout << "	-t" << endl;
	cout << "	--terminal" << endl;
	cout << "		Don't detach Dispatch from the terminal" << endl;
	cout << "" << endl;
}
	
void DispatchMain::loadDefaultArguments() {
	config_file = "dispatch.conf";
	config_dir = "../etc";
}

/**
* @TODO the cmdline argument parser is not a pretty sight. 
* there should be a library capable of doing this?
*/

int DispatchMain::parseCommandLineArguments() {
	vector<string>::iterator iter;
	cout << "Scanning arguments " << endl;
	int state = 10; // Firt argument should be the program name, after that we turn to state 0
	for(iter = ARGV.begin(); iter != ARGV.end(); iter++) {
		string arg = *iter;
		dbg << "State: " << state << ": " << arg << endl;
		switch(state) {
		case 0: // Not in any multi-arg sequence
			if ((arg == "-c") || (arg == "--config-file")) {
				state = 1;
				continue;
			} else if ((arg == "-d") || (arg == "--config-dir")) {
				state = 2;
				continue;
			} else if((arg == "-t") || (arg == "--terminal")) {
				dont_detach = true;
				continue;
			} else if ((arg == "-p") || (arg == "--pidfile")) {
				state = 3;
				continue;
			} if ((arg == "-h") || (arg == "--help")) {
				cmdLineHelp();
				return 2; // Ikke ugyldig men ikk estart opp
			}
			break;
		case 1: // Excpecting file-name
			config_file = arg;
			state = 0;
			continue;
		case 2: // Excepting directory
			config_dir = arg;
			state = 0;
			continue;
		case 3: // Pidfile
			pid_file = arg;
			state = 0;
			continue;
		case 10: // Program name
			program_name = arg;
			state = 0;
			continue;
		} // End switch
		return false;
	} // end foreach
	if (state) {
		/**
		* Hvis vi har state her, så betyr det at det ble fritt for argumenter mitt i en multisekvens
		*/
		return 0; // Ugyldig
	}
	return 1; // Gyldig
}

int DispatchMain::run() {
	loadDefaultArguments();
	int parse_res = parseCommandLineArguments();
	if (!parse_res) {
		err << "Illegal arguments" << endl;
		return 2;
	}
	if (parse_res != 1) {
		// Noe annet en 1 skal vi avslutte her. Sannsynligvis --help
		return 0;
	}
	config::parseDriver drv(getConfigFile(), getConfigDir()); //"dispatch.conf", "../etc");
//	config::parseDriver drv("dispatch.conf", "../etc");
	if (drv.parse()) {
		err << "Bad config" << endl;
		return 1;
	}
//	drv.dump();
	out << "Done parsing" << endl;
	ModuleManager m;
	m.loadModules(drv, modules);

	err << modules.size() << " moduler lastet" << endl;
	
	/**
	* Attach to main process-object
	*/
	for(uint i = 0; i < modules.size(); i++) {
		MOD(i)->setQueueProvider(this);
	}

	/**
	 * Pre-initialize modules
	 */
	for(uint i = 0; i < modules.size(); i++) {
		if (!MOD(i)->preInitialize()) {
			err << "module "<<MOD(i)->getModuleName() << " failed to pre-initialize" << endl;
		} else {
			err << "Pre-initialized module " << MOD(i)->getModuleName() << endl;
		}
	}

	/**
	 * Load config
	 */
	for(uint i = 0; i < modules.size(); i++) {
		GConfigNodeVisitor* visitor = MOD(i)->getConfigVisitor();
		if (visitor != NULL) {
			drv.runVisitor(visitor);
			delete visitor;
		}
	}

	drv.postCheckConfig();
	/**
	 * Create the eventQueue
	 */
//    EventQueue::instance();
    getEventQueue();
	/**
	 * Start'em'up
	 */
	for(uint i = 0; i < modules.size(); i++) {
		if (!MOD(i)->startup()) {
			err << "module " << MOD(i)->getModuleName() << "failed to start up properly, shutting it down" << endl;
			MOD(i)->shutdown();
			MOD(i)->started = false;
		} else {
			MOD(i)->started = true;
			err << "Startet modul " << MOD(i)->getModuleName() << endl;
		}
	}
	if (dont_detach) {
		service();
		return 0;
	}
	int pid = fork();
	if(pid < 0) {
		err << "Unable to fork process" << endl;
		return -1;
	}
	if (pid) {
		err << "in parent process. Child has " << pid << endl;
		return 0;
	}
	service();
	return 0;
}

int DispatchMain::service() {
	/**
	* Start opp socket-handler-tråd
	*/
	FDMonitor::instance().start();
	/**
	 * Kickstart everything by fireing up the eventqueue
	 */

	ThreadFunctor<EventQueue, void*> queue_functor(getEventQueue(), &EventQueue::startListener, &EventQueue::stopListener);
	queue_functor.setName("EventQueue");
	queue_functor.start();

	/**
	* Vent på eventuelle signaler
	*/
	err << "Idling main thread" << endl;
	pause();
	
	err << "Shutting down main thread" << endl;
	
	shutdown(queue_functor);
	return 0;
}

int DispatchMain::shutdown(ThreadFunctor<EventQueue,void*>& queue_functor) {

	FDMonitor::instance().stop();
	/**
	* Steng ned main kø
	*/
	queue_functor.stop();
	/**
	* Close up
	*/
	err << "Dispatch: " << "Stenger ned" << endl;
	
	for(uint i = 0; i < modules.size(); i++) {
		if (!MOD(i)) {
			continue;
		}
		if (MOD(i)->started) {
			MOD(i)->shutdown();
		}
	}
	sleep(1);
//	for(int i = 0; i < modules.size(); i++) {
//	}
	for(uint i = 0; i < modules.size(); i++) {
		modules[i].destroy(MOD(i));
		MOD(i) = NULL;
	}
	return 0;
	

}


}} // end namespace
