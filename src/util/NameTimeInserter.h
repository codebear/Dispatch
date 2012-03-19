#ifndef _NAME_TIME_INSERTER_H
#define _NAME_TIME_INSERTER_H 1

#include "FilteringStreamBuf.h"
#include "Mutex.h"
#include <string>
#include <sstream>

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
	Mutex _lock;
public:
	/**
	* Initialiser med name-provider og stream-buf
	*/
	NameTimeTaggedOutput(NameProvider* n, streambuf* buf);
	virtual ~NameTimeTaggedOutput();
	
	virtual bool lock();
	
	virtual bool unlock();
	
};

class OStreamProxy;


template <class RET_TYPE> 
class _StreamProviderBase {
public:
	typedef RET_TYPE stream_t;
	virtual RET_TYPE out()=0;
	virtual RET_TYPE err()=0;
	virtual RET_TYPE log()=0;
	virtual RET_TYPE dbg(int level=0)=0;
};
/*
class OStreamProxySet : public _StreamProviderBase<OStreamProxy> {

};
*/
/*
class OStreamProxySet {
	virtual OStreamProxy out() = 0;
	virtual OStreamProxy err() = 0;
	virtual OStreamProxy log() = 0;
	virtual OStreamProxy dbg() = 0;
};

*/
/*class NameTimeTaggingOutputSetBase;

class _OutputStreamProvider {
	vector<NameTimeTaggingOutputSetBase> streams;
public:
	void registerStream(NameTimeTaggingOutputSetBase* stream);
	void unregisterStream(NameTimeTaggingOutputSetBase* stream);
	
};

*/


template <class STREAM_PROVIDER_T, class OSTREAM_T>
class NameTimeTaggingOutputSetBase : 
	virtual public NameProvider, 
//	virtual public OStreamProxySet,
	public _StreamProviderBase<OSTREAM_T>
{
protected:
	STREAM_PROVIDER_T* stream_provider;
public:
	NameTimeTaggingOutputSetBase() {};
	NameTimeTaggingOutputSetBase(STREAM_PROVIDER_T s) : stream_provider(s) {}
	
/*	virtual OStreamProxy out() = 0;
	virtual OStreamProxy err() = 0;
	virtual OStreamProxy log() = 0;
	virtual OStreamProxy dbg() = 0;*/
};

class StdStreamBufProvider :
	public _StreamProviderBase<streambuf*> 
{
public:
	virtual stream_t out() { return cout.rdbuf(); }
	virtual stream_t log() { return clog.rdbuf(); }
	virtual stream_t err() { return cerr.rdbuf(); }
	virtual stream_t dbg(int level=0) { return cerr.rdbuf(); }
};


class CustomStreamBufProvider : 
	public _StreamProviderBase<streambuf*> 
{
	stream_t _out, _err, _log, _dbg;
public:
	CustomStreamBufProvider(stream_t o, stream_t e, stream_t l, stream_t d) {
		_out = o;
		_err = e;
		_log = l;
		_dbg = d;
	}
	virtual stream_t out() { return _out; }
	virtual stream_t err() { return _err; }
	virtual stream_t log() { return _log; }
	virtual stream_t dbg(int level=0) { return _dbg; }
};

class DynamicStreamBufObserver {
	virtual void StreamBufsChanged() = 0;
};

class DynamicStreamBufProvider : 
	public _StreamProviderBase<streambuf*>
{
	DynamicStreamBufObserver* observer;
public:
	DynamicStreamBufProvider(DynamicStreamBufObserver* obs) {
	
	}
	virtual stream_t out() { return cout.rdbuf(); }
	virtual stream_t err() { return cerr.rdbuf(); }
	virtual stream_t log() { return clog.rdbuf(); }
	virtual stream_t dbg(int level=0) { return cerr.rdbuf(); }
};


/**
* Komplett sett med stdout, stderr, stdlog og stddbg med nametimetagging
* Abstract klasse som må sub-klasses av noen som trenger den
*/
class NameTimeTaggingOutputSet : 
	public NameTimeTaggingOutputSetBase<_StreamProviderBase<streambuf*>, OStreamProxy>,
	public DynamicStreamBufObserver
	
{
//virtual public NameProvider {
protected:
	/**
	* Stdout
	*/
	NameTimeTaggedOutput* _out;

	/**
	* Stderr
	*/
	NameTimeTaggedOutput* _err;

	/**
	* Stdlog
	*/
	NameTimeTaggedOutput* _log;

	/**
	* Stddbg
	*/
	map<int, NameTimeTaggedOutput*> _dbg;

public:
	/**
	* Instansier med standard
	*/
	NameTimeTaggingOutputSet() 
	:
		_out(0),
		_err(0),
		_log(0)
//		_dbg(0)
	{
		stream_provider = new DynamicStreamBufProvider(this);
	}
	/**
	* Instansier med streambufs
	*/
	NameTimeTaggingOutputSet(streambuf* o, streambuf* e, streambuf* l, streambuf* d) :
		_out(0),
		_err(0),
		_log(0)
//		_dbg(0)
	{
		stream_provider = new CustomStreamBufProvider(o,e,l,d);
/*		_out = new NameTimeTaggedOutput(this, o);
		_err = new NameTimeTaggedOutput(this, e);
		_log = new NameTimeTaggedOutput(this, l);
		_dbg = new NameTimeTaggedOutput(this, d);
*/	}

	~NameTimeTaggingOutputSet() {
		if (stream_provider) {
			delete stream_provider;
		}
	}
	
	void StreamBufsChanged() {
		_out = _err = _log = 0;
		_dbg.clear();
	}
	
	virtual OStreamProxy out();
	
	virtual OStreamProxy err();
	
	virtual OStreamProxy log();
	
	virtual OStreamProxy dbg(int level = 0);
};


class OStreamProxy : public stringstream {
	NameTimeTaggedOutput* _dest;
//	stringstream buf;
	public:
		OStreamProxy(NameTimeTaggedOutput* out);
		OStreamProxy(OStreamProxy const& orig);
		OStreamProxy& operator=(OStreamProxy const&);
		~OStreamProxy();
		
		ostream& operator<<(string s);
		ostream& operator<<(int);
		ostream& operator<<(ostream& (*)(ostream&));
};


}} // end namespace

#endif
