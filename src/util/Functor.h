#ifndef FUNCTOR_H
#define FUNCTOR_H
/**
* Functor-implementasjon. Inspiration og basisimplementasjon hentet fra
* http://www.newty.de/fpt/functor.html
* Utvidet med argument-støtte og tråd-støtte av meg
*/

#include "Thread.h"

namespace dispatch { namespace util {
//-----------------------------------------------------------------------------------------
// 4.2 How to Implement Functors

/**
* abstract base class for functor-objekter
*/
template <class FArg>
class TFunctor
{
public:

	// two possible functions to call member function. virtual cause derived
	// classes will use a pointer to an object and a pointer to a member function
	// to make the function call

	/**
	* Call using operator
	*/
	virtual void operator()(FArg a)=0;

	/**
	* Call using function
	*/
    virtual void Call(FArg a)=0;
};

/**
* Spesialisering dersom man ikke skal ha argumenter
*/
template <>
class TFunctor<void*>
{
public:

	// two possible functions to call member function. virtual cause derived
	// classes will use a pointer to an object and a pointer to a member function
	// to make the function call

	/**
	* Call using operator
	*/
	virtual void operator()()=0;

	/**
	* Call using function
	*/
    virtual void Call()=0;
};

/**
* Functor som peker på en objekt-metode i klassen TClass
*/
template <class TClass, class FArg> 
class TSpecificFunctor : public TFunctor<FArg>
{
private:
	void (TClass::*fpt)(FArg a);   // pointer to member function
      TClass* pt2Object;     // pointer to object
public:

	/**
	* constructor - takes pointer to an object and pointer to a member and stores
	* them in two private variables
	*/
	TSpecificFunctor(TClass* _pt2Object, void (TClass::*_fpt)(FArg a))
    { 
		pt2Object = _pt2Object;  
		fpt=_fpt; 
    };
	
	/**
	* Gjør det mulig å kjøre functoren direkte
	*/
    virtual void operator()(FArg a)
    {
       (*pt2Object.*fpt)(a);              // execute member function
	};
	
	/**
	* Kjør functoren via Call-metoden
	*/
	virtual void Call(FArg a)
    {
    	(*pt2Object.*fpt)(a);             // execute member function
	};
};


/**
* Spesifikasjon dersom man ikke skal ha noen argumenter på functoren
*/
template <class TClass> 
class TSpecificFunctor<TClass, void*> : public TFunctor<void*>
{
private:
	void (TClass::*fpt)();   // pointer to member function
      TClass* pt2Object;     // pointer to object
public:

	/**
	* constructor - takes pointer to an object and pointer to a member and stores
	* them in two private variables
	*/
	TSpecificFunctor(TClass* _pt2Object, void (TClass::*_fpt)())
    { 
		pt2Object = _pt2Object;  
		fpt=_fpt; 
    };
	
	/**
	* override operator "()"
	* Gjør det mulig å kalle functoren direkte
	*/
    virtual void operator()()
    {
       (*pt2Object.*fpt)();              // execute member function
	};
	
	/**
	* Kjør functoren via call-metoden
	*/
	virtual void Call()
    {
    	(*pt2Object.*fpt)();             // execute member function
	};
};

/**
* Tråd functor, kjør en functor i egen tråd
*/
template <class TClass, class TArg>
class ThreadFunctor : public TSpecificFunctor<TClass, TArg>, public Thread {
	TArg arg_passer;
	TSpecificFunctor<TClass, void*>* thread_stop_callback;
public:
	/**
	* Initialiser thread-functor med objekt-peker og peker til metoden.
	*/
	ThreadFunctor(TClass* obj_ptr, void(TClass::*method)(TArg)) : TSpecificFunctor<TClass, TArg>(obj_ptr, method) {
		
	}
	
	/**
	* Initialiser thread-functor med objekt-peker, peker til metoden, og til en "stoppe"-metode som ikke tar argumenter som skal stoppe tråden
	*/
	ThreadFunctor(TClass* obj_ptr, void(TClass::*method)(TArg), void(TClass::*stopper)()) : TSpecificFunctor<TClass, TArg>(obj_ptr, method) {
		this->thread_stop_callback = new TSpecificFunctor<TClass, void*>(obj_ptr, stopper);
	}
	
	~ThreadFunctor() {
		if (this->thread_stop_callback) {
			delete this->_thread_stop_callback;
		}
	}
	
	/**
	* Start tråden med spesisifisert argument
	*/
	void start(TArg a) {
		arg_passer = a;
		Thread::start();
	}
	
	/**
	* Indre kjøre-metode
	*/ 
	void run() {
		Call(arg_passer);
	}

	/**
	* Stopp tråden
	*/
	void stop() {
		if (this->thread_stop_callback != NULL) {
			this->thread_stop_callback->Call();
		}
		Thread::stop();	
	}
};

/**
* Specification for empty arg
* Tilpassing av thread-functor for tomme argumenter
*/
template <class TClass>
class ThreadFunctor<TClass, void*> : public TSpecificFunctor<TClass, void*>, public Thread {
	TSpecificFunctor<TClass, void*>* thread_stop_callback;
public:
	/**
	* Initialiser thread-functor med objekt-peker og peker til metoden.
	*/
	ThreadFunctor(TClass* obj_ptr, void(TClass::*method)()) : TSpecificFunctor<TClass, void*>(obj_ptr, method) {
		this->thread_stop_callback = NULL;
	}
	
	/**
	* Initialiser thread-functor med objekt-peker, peker til metoden, og til en "stoppe"-metode som ikke tar argumenter som skal stoppe tråden
	*/
	ThreadFunctor(TClass* obj_ptr, void(TClass::*method)(), void(TClass::*stopper)()) : TSpecificFunctor<TClass, void*>(obj_ptr, method) {
		this->thread_stop_callback = new TSpecificFunctor<TClass, void*>(obj_ptr, stopper);
	}
	
	~ThreadFunctor() {
		if (this->thread_stop_callback) {
			delete this->thread_stop_callback;
		}
	}
	
	/**
	* indre kjøre-tråd
	*/
	void run() {
		this->Call();
	}
	
	/**
	* Stopp tråden
	*/
	void stop() {
		if (this->thread_stop_callback != NULL) {
			this->thread_stop_callback->Call();
		}
		Thread::stop();	
	}
};

}} // end namespace
#endif
