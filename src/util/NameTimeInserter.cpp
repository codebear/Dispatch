#include "NameTimeInserter.h"
#include <sstream>
#include "Singleton.h"

namespace dispatch { namespace util {



typedef Singleton<Mutex> _NameTimeLock;



NameTimeInserter::NameTimeInserter() : name(NULL) {}

void NameTimeInserter::setNameProvider(NameProvider* n) {
	name = n;
}

string NameTimeInserter::getPrefix() {
	return "[";
}


string NameTimeInserter::getPostfix() {
	stringstream buf;
	buf << "] ";
	if (name != NULL) {
		buf << name->getName();
		buf << ": ";
	}
//	buf << "[";
	return buf.str();
}



NameTimeTaggedOutput::NameTimeTaggedOutput(NameProvider* n, streambuf* buf) : 
	ostream(buf), 
	name(n) 
{
	wrapper = new FilteringOutputStreambuf<NameTimeInserter>(buf);
	wrapper->inserter().setNameProvider(n);
	rdbuf(wrapper);
}

NameTimeTaggedOutput::~NameTimeTaggedOutput()
{
	if (wrapper != NULL) {
		delete wrapper;
	}
}

/**
* @TODO få forskjellig lås for forskjellig utkanal
*/
bool NameTimeTaggedOutput::lock() {
	return _NameTimeLock::instance().lock();
//	return _lock.lock();
}

bool NameTimeTaggedOutput::unlock() {
	return _NameTimeLock::instance().unlock();
//	return _lock.unlock();
}

OStreamProxy NameTimeTaggingOutputSet::out() {
	if (!_out) {
		_out = new NameTimeTaggedOutput(this, stream_provider->out()); //cout.rdbuf());
	}
	return OStreamProxy(_out);
}

OStreamProxy NameTimeTaggingOutputSet::log() {
	if (!_log) {
		_log = new NameTimeTaggedOutput(this, stream_provider->log()); //clog.rdbuf());
	}
	return OStreamProxy(_log);
}

OStreamProxy NameTimeTaggingOutputSet::err() {
	if (!_err) {
		_err = new NameTimeTaggedOutput(this, stream_provider->err()); //;cerr.rdbuf());
	}
	return OStreamProxy(_err);
}

OStreamProxy NameTimeTaggingOutputSet::dbg(int level) {
	if (!_dbg[level]) {
		_dbg[level] = new NameTimeTaggedOutput(this, stream_provider->dbg(level)); // cerr.rdbuf());
	}
	return OStreamProxy(_dbg[level]);
}

OStreamProxy::OStreamProxy(NameTimeTaggedOutput* out) :
	_dest(out)
{
//	cout << "Starter IO Proxy med tagged output " << endl;
}
OStreamProxy::OStreamProxy(OStreamProxy const& o):
	_dest(o._dest)
{
//	cout << "Kopierer IO proxy" << endl;
//	*this << o;
}
OStreamProxy& OStreamProxy::operator=(OStreamProxy const& o) {
	_dest = o._dest;
	return *this;
}

OStreamProxy::~OStreamProxy() {
	_dest->lock();
	*_dest << this->rdbuf();
	_dest->unlock();
}

ostream& OStreamProxy::operator<<(string s) {
//	stringstream ick;
	stringstream* ick = this;
	*ick << s;
//	ick << s;
//	*this << ick;
	return *this;
}

ostream& OStreamProxy::operator<<(int i) {
	stringstream::operator<<(i);
	return *this;
}

ostream& OStreamProxy::operator<<(ostream& (*p)(ostream&)) {
	return (*p)(*this);
//	stringstream::operator<<(p, *this);
//	return *this;
}

/*
ostream& OStreamProxy::operator<<() {
	_dest->lock();
	
	_dest->unlock();
	return *this;
}
*/
}} // end namespace
