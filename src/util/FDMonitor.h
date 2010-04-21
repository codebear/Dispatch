#ifndef FDMONITOR_H
#define FDMONITOR_H 1

#include "Functor.h"
#include "Thread.h"
#include <map>
#include <string>
#include <poll.h>
#include "Mutex.h"
#include "Singleton.h"


using namespace std;
namespace dispatch { namespace util {

/**
* Holde sammen en file-descriptor og en event som har skjedd på den
*/
struct FDMonitorEvent {
	/**
	* File-descriptor event har skjedd på
	*/
	int fd;
	
	/**
	* Hendelsen som har skjedd
	*/
	short event;
};

/**
* Hjelpestruktur som brukes for å kunne finne navnet på en fd-event som har skjedd
*/
class FDEventType {
public:

	/**
	* Event-maske
	*/
	short event;

	/**
	* Event-navn
	*/
	string name;

	/**
	* Event-beskrivelse
	*/
	string desc;
};

/**
* Hjelpeklasse som bare brukes for å finne navnet på en fd-event
*/
class FDEvent {
public:
	/**
	* navnet på eventene i fd-event-masken
	*/
	static string getEventNames(short mask);	
};


/**
* Definere datatypen for callback-handlere
* Det er en functor som tar en fdmonitorevent-peker som argument
*/
typedef TFunctor<FDMonitorEvent*> FDMonitorCallback;

/**
* Knyttning mellom en file-descriptor og en callback, med en event-maske
*/
class FDMonitorEntry {
public:
	/**
	* instansier med fd-som skal overvåkes, masken som skal brukes og callbacken som skal kjøres på eventer
	*/
	FDMonitorEntry(int fd, short mask, FDMonitorCallback* cb);

	/**
	* Holder file-descriptoren
	*/
	int fd;
	
	/**
	* event-maske
	*/
	short mask;
	
	/**
	* Callback for eventer
	*/
	FDMonitorCallback* callback;
};


/**
* implementasjon av en fd-monitor
*/
class _FDMonitor_impl : public Thread {
private:
	Mutex m_lock;
	std::map<int, FDMonitorEntry*> monitored_fds;
	bool doIteration();
public:
	_FDMonitor_impl();
	
	/**
	* Registrer en file-descriptor for eventtilbakemeldinger
	*/
	bool registerFD(int fd, short mask, FDMonitorCallback* callback);
	
	/**
	* Fjern en file-descriptor fra event-tilbakemeldingsarkivet
	*/
	bool unregisterFD(int fd);
	
	/**
	* Kjør monitoren
	*/
	void run();

};

/**
* Lag det til et singleton-klasse
*/
typedef Singleton<_FDMonitor_impl> FDMonitor;





}} // end namespace 

#endif
