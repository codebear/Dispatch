#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <deque>
#include <vector>

using namespace std;


namespace dispatch { namespace util {

/**
* Blocking-queue klasse. Hentet fra nettet, ikke laget selv.
* http://danborn.net/code/blocking_queue.h
*/
template <class value_type> class BlockingQueue {

  deque<value_type *> data;

  unsigned int maxsize;

  /* Flag a thread to stop blocking on an empty queue. */
  bool qdone;             

  /* Controls access to the queue's internal state. */
  pthread_mutex_t qlock;  

  /* Signals threads waiting for the queue to wake up. */
  pthread_cond_t qcond;

  /* Return ((current time) + timeout) in endtime. Return -1 for error. */
  int end_time(struct timespec *endtime /*out*/, struct timespec *timeout);

public:

  /**
   * maxsize == 0 means grow without bound. maxsize > 0 means that push
   * will block while the queue is full. 
   */
  BlockingQueue(unsigned int maxsize = 0);

  virtual ~BlockingQueue();

  /**
  * Return 0 for success, ETIMEDOUT for a time-out, or -1 for other error. 
  */
  int push(value_type *item, struct timespec *timeout = NULL);

  /**
   * Return 0 for success, ETIMEDOUT for a time-out, EWOULDBLOCK if the queue is
   * no longer receiving data and would block indefinitely, or -1 for other
   * error.  timeout == NULL means block indefinitely, or until another thread
   * calls signal_done(). timeout == 0 means immediately return data if there is
   * any available, or otherwise immediately return EWOULDBLOCK. 
   */
  int front(value_type *&item /*out*/, 
            struct timespec *timeout = NULL);

  /**
   * Return 0 for success, or -1 if the queue is empty. 
   */
  int pop();

  /**
   * Return 0 for success, -1 for error. 
   */
  int signal_done();

  /**
   * Return 0 for success, -1 for error. 
   */
  int is_done(bool &done /*out*/);

  /** 
   * Yuck 
   */
  int copy_data(vector<value_type *> &newdata /*out*/);
};

template <class value_type>
BlockingQueue<value_type>::BlockingQueue(unsigned int maxsize) {
  this->maxsize = maxsize;
  this->qdone = false;
  pthread_mutex_init(&this->qlock, NULL);
  pthread_cond_init(&this->qcond, NULL);
}

template <class value_type>
BlockingQueue<value_type>::~BlockingQueue() {
  pthread_mutex_destroy(&this->qlock);
  pthread_cond_destroy(&this->qcond);
}

template <class value_type>
int BlockingQueue<value_type>::end_time(struct timespec *endtime /*out*/, 
                                         struct timespec *timeout) {
  time_t curtime;
  /* Works in Linux, but not OS X:
     clock_gettime(CLOCK_REALTIME, &spec_curtime);*/
  /* More portable, but less accurate: */
  if(time(&curtime) == -1) {
    return -1;
  }
  endtime->tv_nsec = timeout->tv_nsec;
  endtime->tv_sec = timeout->tv_sec + curtime;
  return 0;
}

template <class value_type>
int BlockingQueue<value_type>::push(value_type *item,
                                     struct timespec *timeout) {
  struct timespec maxtime;
  int rv = 0;
  if(timeout != NULL) {
    if(this->end_time(&maxtime, timeout) == -1) {
      return -1;
    }
  }
  pthread_mutex_lock(&this->qlock);
  if(this->maxsize > 0) {
    while(this->data.size() == this->maxsize) {
      if(timeout == NULL) {
        pthread_cond_wait(&this->qcond, &this->qlock);
      } else if(pthread_cond_timedwait(&this->qcond, &this->qlock, &maxtime) ==
                ETIMEDOUT) {
        rv = ETIMEDOUT;
        goto unlock_return;
      }
    }
  }
  this->data.push_back(item);
  this->qdone = false;
  pthread_cond_broadcast(&this->qcond);
 unlock_return:
  pthread_mutex_unlock(&this->qlock);
  return rv;
}

template <class value_type>
int BlockingQueue<value_type>::front(value_type *&item /*out*/,
                                      struct timespec *timeout) {
  struct timespec maxtime;
  int rv = 0;
  if(timeout != NULL) {
    if(this->end_time(&maxtime, timeout) == -1) {
      return -1;
    }
  }
  pthread_mutex_lock(&this->qlock);
  while(!this->qdone && this->data.empty()) {
    if(timeout == NULL) {
      pthread_cond_wait(&this->qcond, &this->qlock);
    } else if(pthread_cond_timedwait(&this->qcond, &this->qlock, &maxtime) ==
              ETIMEDOUT) {
      rv = ETIMEDOUT;
      goto unlock_return;
    }
  }
  if(!this->data.empty()) {
    item = this->data.front();
  } else {
    rv = EWOULDBLOCK;
  }
 unlock_return:
  pthread_mutex_unlock(&this->qlock);
  return rv;
}

template <class value_type>
int BlockingQueue<value_type>::pop() {
  int rv = 0;
  pthread_mutex_lock(&this->qlock);
  if(this->data.empty()) {
    rv = -1;
  } else {
    this->data.pop_front();
  }
  pthread_mutex_unlock(&this->qlock);
  return rv;
}

template <class value_type>
int BlockingQueue<value_type>::signal_done() {
  pthread_mutex_lock(&this->qlock);
  this->qdone = true;
  pthread_cond_broadcast(&this->qcond);
  pthread_mutex_unlock(&this->qlock);
  return 0;
}

template <class value_type>
int BlockingQueue<value_type>::is_done(bool &done /*out*/) {
  pthread_mutex_lock(&this->qlock);
  done = this->qdone;
  pthread_mutex_unlock(&this->qlock);
  return 0;
}

template <class value_type>
int BlockingQueue<value_type>::copy_data(vector<value_type *> &newdata /*out*/)
{
  pthread_mutex_lock(&this->qlock);
  newdata.reserve(this->data.size());
  for(unsigned int i = 0; i < this->data.size(); ++i) {
    newdata.insert(newdata.end(), new value_type(*this->data[i]));
  }
  pthread_mutex_unlock(&this->qlock);
  return 0;
}

}}
#endif
