/**
* Basistemplate for singleton-klasser
* Hentet fra
* http://www.devarticles.com/c/a/Cplusplus/C-plus-plus-In-Theory-The-Singleton-Pattern-Part-2/1/
*/

// singleton.h
#ifndef __SINGLETON_H
#define __SINGLETON_H


/**
* Template som brukes for å wrappe en klasse deklarasjon til en singleton
*/
template <class T>
class Singleton : public T
{
public:
	/**
	* Hent ut singleton-instance av objektet
	*/
	static T& instance() {
		static T _instance;
		return _instance;
	}
	/**
	* Kjekt å ha en typedefinisjon på hva det er vi har arvet fra
	*/
	typedef T type;
private:
	Singleton();          // ctor hidden
	~Singleton();          // dtor hidden
	Singleton(Singleton const&);    // copy ctor hidden
	Singleton& operator=(Singleton const&);  // assign op hidden
};

#endif
// eof
