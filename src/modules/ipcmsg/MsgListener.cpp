#include "MsgListener.h"
#include <iostream>
#include <errno.h>
namespace dispatch { namespace module { namespace ipcmsg {

IPCMsgListener::IPCMsgListener(StreamEventHandler* h, MsgQueue<IPCMsg>* q, long t) :
	NameTimeTaggingOutputSet(),
	Thread("IPCMsgListener"),
	handler(h),
	m_queue(q),
	type(t)
{
	
}


void IPCMsgListener::run() {
	int errcnt = 0;
	while(isRunning() && m_queue->isValid()) {
		out() << "Listening for event with type: " << type << endl;
		IPCMsg m = m_queue->recvMessage(type);
		out() << "Received message IPCMsg" << endl;
		if (m.isValid()) {
			string event = m.getContent();
			if(event.length()) {
				handler->handleCompleteEvent(event);
			}
		} else {
			if (!(++errcnt % 10)) {
				err() << "Passed 10 erroneus attempts. Waiting for some jiffies..." << endl;
				sleep(10);
			}
			if (errcnt > 100) {
				err() << "This is wrong... giving up" << endl;
				break;
			}
		}
	}
	out() << "Closing msgListener. Done." << endl;
}


}}} // end namespace
