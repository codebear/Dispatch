#include "Mutex.h"

namespace dispatch { namespace util {


Mutex::Mutex() {
	pthread_mutex_init(&lock_handle, NULL);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&lock_handle);
}

bool Mutex::lock() {
	return 0 == pthread_mutex_lock(&lock_handle);
}

bool Mutex::trylock() {
	return 0 == pthread_mutex_trylock(&lock_handle);
}

bool Mutex::unlock() {
	return 0 == pthread_mutex_unlock(&lock_handle);
}


}} // end namespace
