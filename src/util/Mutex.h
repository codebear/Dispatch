
#ifndef _MUTEX_H_
#define _MUTEX_H_ 1
#include <pthread.h>
#include "Functor.h"

namespace dispatch { namespace util {


/**
* Wrapper posix-mutexer inn i en mutex-klassen
*/
class Mutex {
	pthread_mutex_t	lock_handle;
	
public:
	Mutex();
	~Mutex();
	/**
	* L�s mutexen, blokker hvis ikke
	*/
	bool lock();
	
	/**
	* L�s mutex, men ikke blokker
	*/
	bool trylock();
	
	/**
	* L�s opp
	*/
	bool unlock();
	
	pthread_mutex_t* getHandle();
};

/**
* Kj�r en functor syncronisert med en mutex
*/
template <class T_ARG> 
void synchronize(Mutex* m, TFunctor<T_ARG>* f, T_ARG arg) {
	if (m->lock()) {
		f(arg);
		m->unlock();
	}
}

/*
//template <>
int synchronize(Mutex* m, TFunctor<void*>* f) {
	if (m->lock()) {
		(*f)();
		m->unlock();
	}
}
*/
/**
* Kj�r en specificfunctor syncronisert med en mutex og med argument
*/
template <class T_CLASS, class T_ARG> 
int synchronize(Mutex* m, TSpecificFunctor<T_CLASS, T_ARG>* f, T_ARG arg) {
	if (m->lock()) {
		f(arg);
		return m->unlock();
	}
	return 0;
}

/**
* Kj�r en specificfunctor synchronisert
*/
template <class T_CLASS> 
int synchronize(Mutex* m, TSpecificFunctor<T_CLASS, void*>* f) {
	if (m->lock()) {
		f();
		return m->unlock();
	}
	return 0;
}


}} // end namespace

#endif

