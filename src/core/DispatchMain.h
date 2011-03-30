
#include "../config/config.h"
#include "EventQueue.h"
#include "QueueProvider.h"

#include <string>
#include <vector>
#include "../modules/DispatchModule.h"
#include "../modules/modules.h"
#include "SigHandler.h"
#include "../util/NameTimeInserter.h"
#include "../util/Functor.h"

using namespace std;
using namespace dispatch;
using namespace dispatch::module;
using namespace dispatch::util;

#define MOD(i) (modules[i].module)
namespace dispatch { namespace core {

/**
* Wrapper-class for the main dispatch-process
*/
class DispatchMain : 
	public SignalRecipient, 
	public NameTimeTaggingOutputSet,
	public QueueProvider {
	vector<string> ARGV;
	vector<ModuleEntry> modules;
	
	string config_file;
	string config_dir;
	string program_name;
	string pid_file;
	
	EventQueue* queue;
	
	bool dont_detach;
	/**
	* Read config arguments from cmd line
	*/
	int parseCommandLineArguments();
	
	/**
	* Initialiser default-verdier for konfig-parametre
	*/
	void loadDefaultArguments();
	
	/**
	* Hent ut filnavn for å lese konfig fra
	*/
	string getConfigFile();
	
	/**
	* Hent ut katalog som skal brukes som basis for henting av konfig
	*/
	string getConfigDir();
	
	/**
	* Dump kommandolinje-hjelp
	*/
	void cmdLineHelp();
public:
	/**
	* Instansier med argument-count og argv-peker til det som kom med programmet
	*/
	DispatchMain(int argc, char* argv[]);
	
	~DispatchMain();		
	
	/**
	* Navn på hoved-tråden 
	*/
	virtual string getName();
	
	/**
	* Handter inngående signaler
	*/ 
	void handleSignal(int sig);
	
	/**
	* Hent ut hoved-event-queuen
	*/ 
	EventQueue* getEventQueue();
	
	/**
	* Start opp hovedprosess
	*/
	int start();
	
	/**
	* Initialisering, kjøres før run
	*/
	void initialize();
	
	/**
	* Opprydding, kjøres etter run
	*/
	void cleanup();
	
	/**
	* Denne skal gjøre hovedarbeidet
	*/
	int run();
	
	int startup();
	
	int service();
	
	int shutdown(ThreadFunctor<EventQueue,void*>& queue_functor);
};

}}

