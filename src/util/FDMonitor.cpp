
#include "FDMonitor.h"
#include <poll.h>
#include <iostream>
#include <errno.h>
#include <sstream>
#include <cstring>

using namespace std;

namespace dispatch { namespace util {


FDMonitorEntry::FDMonitorEntry(int f, short m, FDMonitorCallback* cb) :
	fd(f),
	mask(m),
	callback(cb)
{

}


void _FDMonitor_impl::run() {
	while(this->isRunning()) {
		if (!this->doIteration()) {
			if (!monitored_fds.size()) {
				err() << "Nothing to listen against, so waiting for something to show up..." << endl;
				getCondition()->waitFor();
			}
			err() << "Polling "<< monitored_fds.size() << " returnerte false... venter i 0.5sec og prøver igjen" << endl;
			/**
			* Dersom den returnere false, så 
			* venter vi litt, for å unngå en grum-loop
			*/ 
			usleep(500000);
		}
	}
}

string _FDMonitor_impl::getName() {
	return "FDMonitor";
}

bool _FDMonitor_impl::registerFD(int fd, short mask, FDMonitorCallback* cb) {
	m_lock.lock();
	if (monitored_fds[fd] != NULL) {
		m_lock.unlock();
		return false;
	}
	monitored_fds[fd] = new FDMonitorEntry(fd, mask, cb);
	m_lock.unlock();
	getCondition()->wakeAll();
	return true;
}

bool _FDMonitor_impl::unregisterFD(int fd) {
	m_lock.lock();
	FDMonitorEntry* e = monitored_fds[fd];
	if(e != NULL) {
		monitored_fds.erase(fd);
		delete e;
//		monitored_fds[fd] = NULL;
		m_lock.unlock();
		return true;
	}
	m_lock.unlock();
	return false;
}

_FDMonitor_impl::_FDMonitor_impl() : Thread("FDMonitor") {

}

_FDMonitor_impl::~_FDMonitor_impl() {
    std::map<int, FDMonitorEntry*>::iterator it;
    FDMonitorEntry* entry;
    for(it = monitored_fds.begin(); it != monitored_fds.end(); ++it) {
	entry = it->second;
	if (entry) {
	    delete entry;
	}
    }

}

bool _FDMonitor_impl::doIteration() {
	m_lock.lock();

	int num_fds = monitored_fds.size();
	struct pollfd fds[num_fds];
	map<int, FDMonitorEntry*>::iterator e_it;
	int num = 0;
//	stringstream tmp;
	for(e_it = monitored_fds.begin(); e_it != monitored_fds.end(); e_it++) {
		FDMonitorEntry* entry = (*e_it).second;
		if (entry == NULL) {
			continue;
		}
//		tmp << "[" << entry->fd << "]:" << entry->mask << " ";
		fds[num].fd = entry->fd;
		fds[num].events = entry->mask;
		num++;
	}
	m_lock.unlock();
	int timeout = 10*1000;
	if (!num_fds) {
		return false;
	}
//	out << "Poller " << num << "fds: " << tmp.str() << endl;
	int ret = poll(fds, num, timeout);
	if (ret == -1) {
		err() << "Error polling: " << strerror(errno) << endl;
		return false;
	}
	if (ret == 0) {
		/**
		* Timeout
		*/
		return true;
	}
	for(int i = 0; i < num; i++) {
		struct pollfd* cfd = &fds[i];
		if (cfd->revents == 0) {
//			out << cfd->fd << " har ingen eventer, hopper over" << endl;
			continue;
		}
		FDMonitorEntry* entry = monitored_fds[cfd->fd];
		if (entry == NULL) {
			/**
			* FD må ha blitt fjernet siden sist vi gikk inn i poll...
			* Da hopper vi glatt over
			*/
//			out << cfd->fd << " har forsvunnet fra overvåkningslista, hopper over." << endl;
			continue;
		}
		/**
		* Vi maskerer ut eventer som den ikke (lenger) vil ha (kan i teorien være endret siden vi begynte å lytte)
		* Og dersom noen finnes så kaller vi opp handleren
		*/
//		if (entry->mask & cfd->revents) {
			/**
			* Lag event-objekt
			*/
			FDMonitorEvent* evnt = new FDMonitorEvent();
			evnt->fd = cfd->fd;
			evnt->event = cfd->revents;
			/**
			* Kjøre callback
			*/
			(*entry->callback)(evnt);
			/**
			* Fjerne det igjen. Mulig det hadde gått raskere å hatt et objekt i minnet, som vi gjenbrukte?
			*/
			delete evnt;
//		}
	} // end foreach monitored fds
	
	return true;
}

	



#define EVNT_TYPE(a, b) {a, #a, b}


string FDEvent::getEventNames(short events) {
	struct FDEventType event_enum[] = {		
		EVNT_TYPE(
			POLLIN,
			"Data other than high-priority data may be read without blocking. For STREAMS, this flag is set in revents even if the message is of zero length."
		),
		EVNT_TYPE(
			POLLRDNORM,
			"Normal data (priority band equals 0) may be read without blocking. For STREAMS, this flag is set in revents even if the message is of zero length."
		),
		EVNT_TYPE(
			POLLRDBAND,
			"Data from a non-zero priority band may be read without blocking. For STREAMS, this flag is set in revents even if the message is of zero length."
		),
		EVNT_TYPE(
			POLLPRI,
			"High-priority data may be received without blocking. For STREAMS, this flag is set in revents even if the message is of zero length."
		),
		EVNT_TYPE(
			POLLOUT,
			"Normal data (priority band equals 0) may be written without blocking."
		),
		EVNT_TYPE(
			POLLWRNORM,
			"Same as POLLOUT."
		),
		EVNT_TYPE(
			POLLWRBAND,
			"Priority data (priority band > 0) may be written. If any priority band has been written to on this STREAM, this event only examines bands that have been written to at least once. "
		),
		EVNT_TYPE(
			POLLERR,
			"An error has occurred on the device or stream. This flag is only valid in the revents bitmask; it is ignored in the events member."
		),
		EVNT_TYPE(
			POLLHUP,
			"The device has been disconnected. This event and POLLOUT are mutually exclusive; a stream can never be writable if a hangup has occurred. However, this event and POLLIN, POLLRDNORM, POLLRDBAND or POLLPRI are not mutually exclusive. This flag is only valid in the revents bitmask; it is ignored in the events member. "
		),
		EVNT_TYPE(
			POLLNVAL,
			"The specified fd value is invalid. This flag is only valid in the revents member; it is ignored in the events member."
		)
	};


	
	int event_cnt = sizeof(event_enum)/sizeof(event_enum[0]);
//	out << "Leter igjennom " << event_cnt << " elementer etter hvilke som er på" << endl;
	int hit_cnt = 0;
	stringstream buf;
	for(int i = 0; i < event_cnt; i++) {
		if (events & event_enum[i].event) {
			if (hit_cnt++) {
				buf << ", ";
			}
			buf << event_enum[i].name;
			/**
			* Slå av bitten
			*/
			events ^= event_enum[i].event;
		}
	}
	if (events != 0) {
		if (hit_cnt) {
			buf << ", ";
		}
		buf << "<UNIDENTIFIED EVENTS>";
	}
	return buf.str();
}



}} // end namespace

