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
	* Vi overloader getPrefix for å lage vår egen prefix-string
	*/
	virtual string getPrefix();
	
	/**
	* Vi overloader getPostfix for å lage vår egen postfix-string
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
	virtual ~NameTimeTaggedOutput();
	
};


/**
* Komplett sett med stdout, stderr, stdlog og stddbg med nametimetagging
* Abstract klasse som må sub-klasses av noen som trenger den
*/
class NameTimeTaggingOutputSet : virtual public NameProvider {
public:
	/**
	* Stdout
	*/
	NameTimeTaggedOutput out;
	
	/**
	* Stderr
	*/
	NameTimeTaggedOutput err;

	/**
	* Stdlog
	*/
	NameTimeTaggedOutput log;
	
	/**
	* Stddbg
	*/
	NameTimeTaggedOutput dbg;
	
	/**
	* Instansier med standard
	*/
	NameTimeTaggingOutputSet() :
		out(this, cout.rdbuf()),
		err(this, cerr.rdbuf()),
		log(this, clog.rdbuf()),
		dbg(this, cerr.rdbuf()) {
	
	}
	/**
	* Instansier med streambufs
	*/
	NameTimeTaggingOutputSet(streambuf* o, streambuf* e, streambuf* l, streambuf* d) : 
		out(this, o),
		err(this, e),
		log(this, l),
		dbg(this, d) {
	}
};


}} // end namespace

#endif
