/*
 * Thread.cpp
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */

#include "Thread.h"
#include <unistd.h>
#include <string>
#include <sys/prctl.h>
#include <iostream>
#include <cstring>
#include <errno.h>
#include "Mutex.h"
#include <sys/time.h>

namespace dispatch { namespace util {

Thread::Thread(std::string n) : 
	name(n),
	default_condition(0)
{

}

Thread::Thread() :
	default_condition(0) {
}

Thread::~Thread() {
	if (conditions.size() > 0) {
		map<string, ThreadCondition*>::iterator conds;
		for(conds = conditions.begin(); conds != conditions.end(); ++conds) {
			delete (*conds).second;
		}
	}
	if (default_condition) {
		delete default_condition;
	}
}



void Thread::_global_initialize() {
	pthread_key_create(&_thread_key, NULL);
}

bool Thread::currentThreadIsRunning() {
	Thread* curr = Thread::currentThread();
	if (curr != NULL) {
		return curr->isRunning();
	}
	return false;
}

pthread_key_t Thread::_thread_key;

pthread_once_t Thread::_run_once = PTHREAD_ONCE_INIT;

vector<Thread*> Thread::threads;

void Thread::globalInitialize() {
	pthread_once(&_run_once, &Thread::_global_initialize);
}

void Thread::threadInitialize() {
	void* ptr = pthread_getspecific(_thread_key);
	if (ptr == NULL) {
		pthread_setspecific(_thread_key, (void*) this);
	}
	_actuallySetName();

}

Thread* Thread::currentThread() {
	void* ptr = pthread_getspecific(_thread_key);
	if (ptr != NULL) {
		return (Thread*)(ptr);
	}
	return NULL;
}


void Thread::setRunning(bool b) {
	if (b) {
		_stopped = false;
	}
	_running = b;
}

void Thread::stop() {
	setRunning(false);
}

void Thread::start() {
	globalInitialize();
	setRunning(true);

	void*(*thread_handler)(void*) = &Thread::_run;

//	ThreadWrapper*
	ThreadWrapper* wrapper = new ThreadWrapper(this);


	int res = pthread_create(&thread_id, NULL, thread_handler, wrapper);
	
	if (res == 0) {
		// Success
	}

}

void* Thread::_run(void* thread_wrap_v) {
	ThreadWrapper* t_wrap = static_cast<ThreadWrapper*>(thread_wrap_v);
	Thread* thread = t_wrap->thread;
	delete t_wrap;
	thread->threadInitialize();
	thread->run();
	thread->_stopped = true;
	pthread_exit(NULL);
}

bool Thread::isRunning() {
	return _running;
}

bool Thread::isStopped() {
	return _stopped;
}

void Thread::sleep(int sec) {
	::sleep(sec);
}

void Thread::usleep(int usec) {
	::usleep(usec);
}

void Thread::setName(std::string s) {
	name = s;
}

void Thread::_actuallySetName() {
	if (name.length()) {
//		std::cout << "Setting thread name: " << name << std::endl;
		std::string name_s = name.substr(0, 15);
		int ret = prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
		if (ret) {
			std::cerr << "Error setting name: " << strerror(errno) << std::endl;
		}
	}
}

ThreadCondition* Thread::getCondition(string key) {
	ThreadCondition* c = conditions[key];
	if (!c) {
		c = new ThreadCondition(this);
		conditions[key] = c;
	}
	return c;
}

ThreadCondition* Thread::getCondition() {
	if (!default_condition) {
		default_condition = new ThreadCondition(this);
	}
	return default_condition;
}

/**
* ThreadConditions
*/
ThreadCondition::ThreadCondition(Thread* t) :
	thread(t),
	_aquired(false)
{
	pthread_cond_init(&condition, NULL);
	pthread_mutex_init(&mutex, NULL);
}

ThreadCondition::~ThreadCondition() {
	pthread_cond_destroy(&condition);
	pthread_mutex_destroy(&mutex);
}

/**
* Returnerer antall millisekund ventet, eller -1 dersom timeout
*/
long ThreadCondition::waitFor(long usleep) {
	struct timeval now;
//	struct timeval timeout;
	struct timeval done;

	if (!_aquired) {
		pthread_mutex_lock(&mutex);
	}
	gettimeofday(&now, NULL);
	
//	int s = usleep/1000000;
//	int us = usleep - s*1000000;
	
//	timeout.tv_sec = now.tv_sec + s;
//	timeout.tv_usec = now.tv_usec + us;
	
	int ret = pthread_cond_wait(&condition, &mutex);
	gettimeofday(&done, NULL);
	
	if (!_aquired) {
		pthread_mutex_unlock(&mutex);
	}
	if (ret == ETIMEDOUT) {
		// timeout
		return -1;
	}
	long elapsed = (done.tv_sec-now.tv_sec)+1000000;
	elapsed += done.tv_usec-now.tv_usec;
	return elapsed;
}

long ThreadCondition::waitFor() {
	if (!_aquired) {
		pthread_mutex_lock(&mutex);
	}
	int c = pthread_cond_wait(&condition, &mutex);
	if (!_aquired) {
		pthread_mutex_unlock(&mutex);
	}
	return c;
}

int ThreadCondition::wakeOne() {
	if (!_aquired) {
		pthread_mutex_lock(&mutex);
	}
	int r = pthread_cond_signal(&condition);
	if (!_aquired) {
		pthread_mutex_unlock(&mutex);
	}
	return r;
}

int ThreadCondition::wakeAll() {
	if (!_aquired) {
		pthread_mutex_lock(&mutex);
	}
	int r = pthread_cond_broadcast(&condition);
	if (!_aquired) {
		pthread_mutex_unlock(&mutex);
	}
	return r;
}

bool ThreadCondition::aquire() {
	int r = pthread_mutex_lock(&mutex);
	if (r != EINVAL) {
		return _aquired = true;
	}
	return false;
}

bool ThreadCondition::release() {
	_aquired = false;
	int r = pthread_mutex_unlock(&mutex);
	return r != EINVAL;
}


}}
