#ifndef _NAME_TIME_INSERTER_H
#define _NAME_TIME_INSERTER_H 1

#include "FilteringStreamBuf.h"
#include <string>

namespace dispatch { namespace util {

/**
* Klasse som kan rapportere et navn
*/
class NameProvider {
public:
	/**
	* hent ut navnet som skal prefixe linjene
	*/
	virtual string getName() = 0;
};

/**
* Stream-formatter som markerer en stream med navn og timestamp
*/
class NameTimeInserter : public TimeStampInserter {
	NameProvider* name;
public:
	NameTimeInserter();

	/**
	* Sett en name-provider for streamen
	*/
	virtual void setNameProvider(NameProvider* n);
	
	/**
	* Vi overloader getPrefix for � lage v�r egen prefix-string
	*/
	virtual string getPrefix();
	
	/**
	* Vi overloader getPostfix for � lage v�r egen postfix-string
	*/
	virtual string getPostfix();
	
};

/**
* Output-stream som tar en name-time-formatter stream
*/
class NameTimeTaggedOutput : public ostream {
	NameProvider* name;
	FilteringOutputStreambuf<NameTimeInserter>* wrapper;
public:
	/**
	* Initialiser med name-provider og stream-buf
	*/
	NameTimeTaggedOutput(NameProvider* n, streambuf* buf);
	virtual ~NameTimeTaggedOutput() {
		if (wrapper != NULL) {
			delete wrapper;
		}
	}
};


/**
* Komplett sett med stdout, stderr, stdlog og stddbg med nametimetagging
* Abstract klasse som m� sub-klasses av noen som trenger den
*/
class NameTimeTaggingOutputSet : virtual public NameProvider {
public:
	/**
	* Stdout
	*/
	NameTimeTaggedOutput out;
	
	/**
	* Stdlog
	*/
	NameTimeTaggedOutput log;
	
	/**
	* Stderr
	*/
	NameTimeTaggedOutput err;
	
	/**
	* Stddbg
	*/
	NameTimeTaggedOutput dbg;
	
	/**
	* Instansier med streambufs
	*/
	NameTimeTaggingOutputSet(streambuf* o, streambuf* e, streambuf* l, streambuf* d) : 
		out(this, o),
		log(this, l),
		dbg(this, d),
		err(this, e) {
	}
};


}} // end namespace

#endif