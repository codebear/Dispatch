#include "Mutex.h"

namespace dispatch { namespace util {


Mutex::Mutex() {
	pthread_mutex_init(&lock_handle, NULL);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&lock_handle);
}

bool Mutex::lock() {
	bool res = (0 == pthread_mutex_lock(&lock_handle));
	_locked = res;
	return res;
}

bool Mutex::trylock() {
	bool res = (0 == pthread_mutex_trylock(&lock_handle));
	if (res) {
		_locked = res;
	}
	return res;
}

bool Mutex::unlock() {
	bool res = (0 == pthread_mutex_unlock(&lock_handle));
	if (res) {
		_locked = false;
	}
	return res;
}

bool Mutex::locked() {
	return _locked;
}

pthread_mutex_t* Mutex::getHandle() {
	return &lock_handle;
}


}} // end namespace
