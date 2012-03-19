/*
 * Thread.h
 *
 *  Created on: May 20, 2009
 *      Author: bear
 *
 * Enkel objekt-wrapper rundt det prosedyrale posix-tr�d API-et.
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <pthread.h>
#include <string>
#include <map>
#include <vector>
//#include "Mutex.h"

using namespace std;
namespace dispatch { namespace util {


class Thread;

//class Mutex;

/**
* Denne er selv-låsende/opplåsende. Dersom man benytter aquire/release, så må benytte det fullt ut. Da kobles automatikken ut.
* Det er nyttig dersom man skal gjøre ting bare dersom man vet man får gå i ventemodus, samt etter at man vet at man har blitt
* vekket, men før noen andre slipper til.
*/

class ThreadCondition {
	Thread* thread;
	pthread_cond_t condition;
	bool _aquired;
	void inline init();
	pthread_mutex_t mutex;
//	Mutex* mutex;

public:
	ThreadCondition(Thread* t);

	virtual ~ThreadCondition();

	/**
	* Wait for condition
	*/ 
	long waitFor();

	/**
	* Wait for condition, at most usleep microseconds
	*/
	long waitFor(long usleep);

	int wakeOne();

	int wakeAll();
	
	/**
	* Lås på forhånd før venting
	*/
	bool aquire();
	
	/**
	* Lås opp manuelt etter utført jobb. Må brukes hvis aquire er brukt.
	*/
	bool release();

};

/*
class _ThreadState {
	Thread* thread;
	bool 
};

*/
/**
* Basisklasse for en tr�d, Abstrakt.
* Bassert p� posix_thread-biblioteket. Fant ingen C++ wrappere for denne (oppdaget vel etterhvert at boost har en), s� lagde en liten en selv.
*/ 

class Thread {
	/**
	* Kj�rer tr�den?
	*/
	bool _running;
	
	/**
	* Er tr�den stoppet?
	*/
	bool _stopped;
	
	/**
	* Handle for posix-tr�den
	*/
	pthread_t thread_id;
	
	/**
	* Handle-struktur som brukes som en transport�r for argumenter inn til tr�d-init-metoden 
	* Se pthread_create
	*/
	struct ThreadWrapper {
		ThreadWrapper(Thread* t) {
			thread = t;
		}
		Thread* thread;
	}; // *wrapper;
	
	/**
	* Intern oppstartsmetode som brukes for � starte tr�den
	*/
	static void* _run(void* thread_wrapper);
	
	/**
	* Global initialiseringsmetode
	*/
	static void _global_initialize();
	
	static pthread_once_t _run_once;
	static pthread_key_t _thread_key;
	
	static vector<Thread*> threads;

	void globalInitialize();
	
	void threadInitialize();
	
	/**
	* Kj�res n�r tr�den er kommet i gang for � f� satt tr�dnavnet i systemet
	*/
	void _actuallySetName();

	/**
	* Navnet p� tr�den
	*/
	std::string name;
	
	map<string,ThreadCondition*> conditions;
	
	ThreadCondition* default_condition;
	
public:
	Thread();
	/**
	* Initialiser en tr�d med navn
	*/
	Thread(std::string n);
	
	virtual ~Thread();
	
	/**
	* Hent ut et thread-objekt for tr�den vi er i n�
	*/
	static Thread* currentThread();
	
	/**
	* Har gjeldene tr�d kj�re-flagget p�
	*/
	static bool currentThreadIsRunning();
	
	/**
	* Setter et flag for om tr�den �nsker � v�re kj�rende eller ikke
	*/
	void setRunning(bool b);

	/**
	* Er det �nskelig at tr�den skal kj�re.
	*/
	virtual bool isRunning();
	
	/**
	* Er tr�den faktisk stoppet. Kan g� en stund fra den blir bedt om � stoppe til det skjer. (Evighet i verste fall)
	*/
	virtual bool isStopped();

	/**
	* Start tr�den
	*/
	virtual void start();
	
	/**
	* Fors�k � stoppe tr�den
	*/
	virtual void stop();

	/**
	* Selve kj�re-metoden som sub-klasser m� implementere 
	*/ 
	virtual void run()=0;

	/**
	* Sett navnet p� tr�den
	*/
	virtual void setName(std::string name);
	
	ThreadCondition* getCondition();
	
	ThreadCondition* getCondition(string key);
	

	/**
	* F� tr�den til � sove i n sekunde
	*/
	static void sleep(int sec);
	
	/**
	* F� tr�den til � sove i n microsekund
	*/
	static void usleep(int usec);
	
};

}}
#endif /* THREAD_H_ */
