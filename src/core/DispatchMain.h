
#include "../config/config.h"
#include "EventQueue.h"

#include <string>
#include <vector>
#include "../modules/DispatchModule.h"
#include "../modules/modules.h"
#include "SigHandler.h"
#include "../util/NameTimeInserter.h"

using namespace std;
using namespace dispatch;
using namespace dispatch::module;
using namespace dispatch::util;

#define MOD(i) (modules[i].module)
namespace dispatch { namespace core {

/**
* Wrapper-class for the main dispatch-process
*/
class DispatchMain : public SignalRecipient, public NameTimeTaggingOutputSet {
	vector<string> ARGV;
	vector<ModuleEntry> modules;
public:
	/**
	* Instansier med argument-count og argv-peker til det som kom med programmet
	*/
	DispatchMain(int argc, char* argv[]);
	
	~DispatchMain();		
	
	/**
	* Navn p� hoved-tr�den 
	*/
	virtual string getName();
	
	/**
	* Handter inng�ende signaler
	*/ 
	void handleSignal(int sig);
	
	/**
	* Start opp hovedprosess
	*/
	int start();
	
	/**
	* Initialisering, kj�res f�r run
	*/
	void initialize();
	
	/**
	* Opprydding, kj�res etter run
	*/
	void cleanup();
	
	/**
	* Denne skal gj�re hovedarbeidet
	*/
	int run();
};

}}

