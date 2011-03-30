/*
 * Thread.h
 *
 *  Created on: May 20, 2009
 *      Author: bear
 *
 * Enkel objekt-wrapper rundt det prosedyrale posix-tråd API-et.
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <pthread.h>
#include <string>
#include <map>
//#include "Mutex.h"

using namespace std;
namespace dispatch {namespace util {


class Thread;

class Mutex;

class ThreadCondition {
	Thread* thread;
	pthread_cond_t condition;
	bool _inited;
	void inline init();
	Mutex* mutex;
public:
	ThreadCondition(Thread* t);
	
	virtual ~ThreadCondition();
	
	int sleepUntil();
	
	int sleepUntil(Mutex* m);
	
	int wakeOne();
	
	int wakeAll();
	

};


/**
* Basisklasse for en tråd, Abstrakt.
* Bassert på posix_thread-biblioteket. Fant ingen C++ wrappere for denne, så lagde en liten en selv.
*/ 

class Thread {
	/**
	* Kjører tråden?
	*/
	bool _running;
	
	/**
	* Er tråden stoppet?
	*/
	bool _stopped;
	
	/**
	* Handle for posix-tråden
	*/
	pthread_t thread_id;
	
	/**
	* Handle-struktur som brukes som en transportør for argumenter inn til tråd-init-metoden 
	* Se pthread_create
	*/
	struct ThreadWrapper {
		ThreadWrapper(Thread* t) {
			thread = t;
		}
		Thread* thread;
	}; // *wrapper;
	
	/**
	* Intern oppstartsmetode som brukes for å starte tråden
	*/
	static void* _run(void* thread_wrapper);
	
	/**
	* Global initialiseringsmetode
	*/
	static void _global_initialize();
	
	static pthread_once_t _run_once;
	static pthread_key_t _thread_key;

	void globalInitialize();
	
	void threadInitialize();
	
	/**
	* Kjøres når tråden er kommet i gang for å få satt trådnavnet i systemet
	*/
	void _actuallySetName();

	/**
	* Navnet på tråden
	*/
	std::string name;
	
	map<string,ThreadCondition*> conditions;
	
	ThreadCondition* default_condition;
	
public:
	Thread();
	/**
	* Initialiser en tråd med navn
	*/
	Thread(std::string n);
	
	virtual ~Thread();
	
	/**
	* Hent ut et thread-objekt for tråden vi er i nå
	*/
	static Thread* currentThread();
	
	/**
	* Har gjeldene tråd kjøre-flagget på
	*/
	static bool currentThreadIsRunning();
	
	/**
	* Setter et flag for om tråden ønsker å være kjørende eller ikke
	*/
	void setRunning(bool b);

	/**
	* Er det ønskelig at tråden skal kjøre.
	*/
	virtual bool isRunning();
	
	/**
	* Er tråden faktisk stoppet. Kan gå en stund fra den blir bedt om å stoppe til det skjer. (Evighet i verste fall)
	*/
	virtual bool isStopped();

	/**
	* Start tråden
	*/
	virtual void start();
	
	/**
	* Forsøk å stoppe tråden
	*/
	virtual void stop();

	/**
	* Selve kjøre-metoden som sub-klasser må implementere 
	*/ 
	virtual void run()=0;

	/**
	* Sett navnet på tråden
	*/
	virtual void setName(std::string name);
	
	ThreadCondition* getCondition();
	
	ThreadCondition* getCondition(string key);
	

	/**
	* Få tråden til å sove i n sekunde
	*/
	static void sleep(int sec);
	
	/**
	* Få tråden til å sove i n microsekund
	*/
	static void usleep(int usec);
	
};

}}
#endif /* THREAD_H_ */
