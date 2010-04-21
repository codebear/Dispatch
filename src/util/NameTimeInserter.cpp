#include "NameTimeInserter.h"
#include <sstream>

namespace dispatch { namespace util {



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


NameTimeTaggedOutput::NameTimeTaggedOutput(NameProvider* n, streambuf* buf) : ostream(buf), name(n) {
	wrapper = new FilteringOutputStreambuf<NameTimeInserter>(buf);
	wrapper->inserter().setNameProvider(n);
	rdbuf(wrapper);
}

}} // end namespace
