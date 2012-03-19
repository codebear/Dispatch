#include "INotifyListener.h"
#include <sys/inotify.h>
#include <vector>
#include <map>
#include <sstream>
#include "../../core/EventQueue.h"
#include <iostream>

// #define foreach(type, coll, ptr) for(type::iterator ptr = (coll).begin(); (ptr) != (coll).end(); (ptr)++)

using namespace std;

namespace dispatch {
namespace module {
namespace inotify {
	INotifyListener::INotifyListener(DispatchModule* mod) : 
		Thread("INotifyListener"),
//		NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()),
		module(mod)
	{
		
	}
	
	INotifyListener::~INotifyListener() {
		
	}
	
	DispatchModule* INotifyListener::getModule() {
		return module;
	}
	
	string INotifyListener::getName() {
		return "INotifyListener";
	}
	
	bool INotifyListener::addWatch(config::NodeIdent& id, string path) {
		watch_paths[path] = id;
		return true;
	}
	
	void INotifyListener::run() {
		int handle = inotify_init();
		if (handle < 0) {
			err() << "inotify_init failed: " << strerror(errno) << endl;
			return;
		}
		map<string, config::NodeIdent>::iterator path;
//		vector<string>::iterator path;
		int mask = IN_MODIFY|IN_MOVED_FROM|IN_MOVED_TO|IN_DELETE|IN_CREATE|IN_DELETE_SELF|IN_ACCESS|IN_ATTRIB;
		map<int, WatchDeclaration> watches;
//		map<int, pair<string, config::NodeIdent> > watches;
		
		for(path = watch_paths.begin(); path != watch_paths.end(); path++) {
			WatchDeclaration decl;
			decl.id = (*path).second;
			decl.path = (*path).first;
			
			dbg() << "Legger til overvåkning av " << decl.path << endl;
			int w = inotify_add_watch(handle, decl.path.c_str(), mask);
			if (w < 0) {
				err() << "inotify_add_watch: " << decl.path << " failed" << endl;
			}
			decl.watch_descriptor = w;
			watches[w] = decl;
		}
		
		err() << "total of " << watches.size() << "watches initialized. Starting listener" << endl;
		
		while(isRunning()) {
			readEvents(handle, watches);
		}
		
		err() << "listener stopping" << endl;
		
		map<int, WatchDeclaration >::iterator w_iter;
		for(w_iter = watches.begin(); w_iter != watches.end(); w_iter++) {
			inotify_rm_watch(handle, (*w_iter).second.watch_descriptor);
		}
		
	}
	
	string INotifyListener::i2s(int i) {
		stringstream conv;
		conv << i;
		return conv.str();
	}
	
	string INotifyListener::namesForMask(int mask) {
		stringstream names;
		if (mask & IN_MODIFY) {
			names << "IN_MODIFY ";
		}
		if (mask & IN_MOVED_FROM) {
			names << "IN_MOVED_FROM ";
		}
		if (mask & IN_MOVED_TO) {
			names << "IN_MOVED_TO ";
		}
		if (mask & IN_DELETE) {
			names << "IN_DELETE ";
		}
		if (mask & IN_CREATE) {
			names << "IN_CREATE ";
		}
		if (mask & IN_DELETE_SELF) {
			names << "IN_DELETE_SELF ";
		}
		if (mask & IN_ACCESS) {
			names << "IN_ACCESS ";
		}
		if (mask & IN_ATTRIB) {
			names << "IN_ATTRIB ";
		}
		return names.str();
	}
	
	void INotifyListener::readEvents(int fd, map<int, WatchDeclaration > & watches) {
		/**
		* Hentet fra http://www.linuxjournal.com/article/8478?page=0,1
		*/
		
		/* size of the event structure, not counting name */
#define EVENT_SIZE  (sizeof (struct inotify_event))
	
		/* reasonable guess as to size of 1024 events */
#define BUF_LEN        (1024 * (EVENT_SIZE + 16))

		char buf[BUF_LEN];
		int len, i = 0;
	
		err() << "Preparing for reading from inotify fd" << endl;
		len = read (fd, buf, BUF_LEN);
		err() << "Read " << len << "bytes from fd" << endl;
		if (len < 0) {
	        if (errno  == EINTR) {
				/* need to reissue system call */
				perror ("inotify read resulted in EINTR");
			} else {
				perror ("inotify read");
			}
		} else if (!len) {
			/* BUF_LEN too small? */
		}
		err() << "looping results from read" << endl;
		
		EventQueue* queue = getModule()->getEventQueue();
		while (i < len) {
			struct inotify_event *event;
			
			event = (struct inotify_event *) &buf[i];
			
			Event* evnt = new Event();

			evnt->setParameter("events_trigged", namesForMask(event->mask));
			evnt->setParameter("watched_path", watches[event->wd].path);
			evnt->setParameter("mask", i2s(event->mask));
			evnt->setParameter("cookie", i2s(event->cookie));
			evnt->setParameter("len", i2s(event->len));
			
			out() << "wd=" << event->wd
				<< " mask=" << event->mask
				<< " cookie=" << event->cookie
				<< " len=" << event->len
				<< endl;
			/*	
			printf ("wd=%d mask=%u cookie=%u len=%u\n",
				event->wd, event->mask,
				event->cookie, event->len);
			*/
				
			if (event->len) {
				evnt->setParameter("name", string(event->name));
				out() << "name=" << event->name << endl;
//				printf ("name=%s\n", event->name);
			}

			err() << "Queue event" << endl;
			queue->queue(evnt, watches[event->wd].id);
			err() << "Seeking " << EVENT_SIZE+event->len << "bytes into buffer for next event" << endl;
			i += EVENT_SIZE + event->len;
		} // end while
		err() << "Done looping events" << endl;
		
	} // end function
	
	
}}}

