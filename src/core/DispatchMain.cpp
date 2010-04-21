
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
	NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()) {
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
int DispatchMain::run() {
	config::parseDriver drv("dispatch.conf", "../etc");
	if (drv.parse()) {
		out << "Bad config" << endl;
		return 1;
	}
	drv.dump();
	out << "Done parsing" << endl;
	ModuleManager m;
	m.loadModules(drv, modules);

	err << modules.size() << " moduler lastet" << endl;

	/**
	 * Pre-initialize modules
	 */
	for(int i = 0; i < modules.size(); i++) {
		if (!MOD(i)->preInitialize()) {
			err << "module "<<MOD(i)->getModuleName() << " failed to pre-initialize" << endl;
		} else {
			err << "Pre-initialized module " << MOD(i)->getModuleName() << endl;
		}
	}

	/**
	 * Load config
	 */
	for(int i = 0; i < modules.size(); i++) {
		GConfigNodeVisitor* visitor = MOD(i)->getConfigVisitor();
		if (visitor != NULL) {
			drv.runVisitor(visitor);
			delete visitor;
		}
	}

	drv.postCheckConfig();
	/**
	 * Create the eventqueue
	 */
    Eventqueue::instance();
	/**
	 * Start'em'up
	 */
	for(int i = 0; i < modules.size(); i++) {
		if (!MOD(i)->startup()) {
			err << "module " << MOD(i)->getModuleName() << "failed to start up properly, shutting down" << endl;
			MOD(i)->shutdown();
			MOD(i)->started = false;
		} else {
			MOD(i)->started = true;
			err << "Startet modul " << MOD(i)->getModuleName() << endl;
		}
	}
	
	/**
	* Start opp socket-handler-tråd
	*/
	FDMonitor::instance().start();
	/**
	 * Kickstart everything by fireing up the eventqueue
	 */

	ThreadFunctor<Eventqueue, void*> queue_functor(Eventqueue::instance(), &Eventqueue::startListener, &Eventqueue::stopListener);
	queue_functor.setName("EventQueue");
	queue_functor.start();

	/**
	* Vent på eventuelle signaler
	*/
	pause();
	
	
	FDMonitor::instance().stop();
	/**
	* Steng ned main kø
	*/
	queue_functor.stop();
	/**
	* Close up
	*/
	err << "Dispatch: " << "Stenger ned" << endl;
	
	for(int i = 0; i < modules.size(); i++) {
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
	for(int i = 0; i < modules.size(); i++) {
		modules[i].destroy(MOD(i));
		MOD(i) = NULL;
	}
	return 0;
	
}


}} // end namespace
