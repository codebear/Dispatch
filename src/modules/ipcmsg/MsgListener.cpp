#include "MsgListener.h"
#include <iostream>
#include <errno.h>
namespace dispatch { namespace module { namespace ipcmsg {

MsgListener::MsgListener(MsgQueue<Msg>* q, long t) :
	Thread("MsgListener"),
	NameTimeTaggingOutputSet(cout.rdbuf(), cerr.rdbuf(), clog.rdbuf(), cerr.rdbuf()),
	queue(q),
	type(t) {
	
}


void MsgListener::run() {
	int errcnt = 0;
	while(isRunning() && queue->isValid()) {
		out << "Listening for event with type: " << type << endl;
		Msg m = queue->recvMessage(type);
		out << "Received message IPCMsg" << endl;
		if (m.isValid()) {
			string event = m.getContent();
			if(event.length()) {
				handler.handleCompleteEvent(event);
			}
		} else {
			if (!(++errcnt % 10)) {
				err << "Passed 10 erroneus attempts. Waiting for some jiffies..." << endl;
				sleep(10);
			}
			if (errcnt > 100) {
				err << "This is wrong... giving up" << endl;
				break;
			}
		}
	}
	out << "Closing msgListener. Done." << endl;
}


}}} // end namespace
