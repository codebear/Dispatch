#ifndef _SYSV_MSG_QUEUE_H
#define _SYSV_MSG_QUEUE_H 1

#include "MsgQueue.h"
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>

/**
* Meldingskø for SysV meldingskøer. Meldinger mottas med klasse T
*/
template <class T>
class SysVMsgQueue : public MsgQueue<T> {
	/**
	* Spesifikk verdi for udefinert kø
	*/
	static const int QUEUE_UNDEF = -100;
	
	/**
	* Keyen til køen vi skal lytte fra
	*/
	key_t msg_key;
	
	/**
	* Holder for meldings-kø-id
	*/
	int msg_queue_id;
	
	/**
	* Returner en sysv-msg-queue-id
	*/
	int getQueueId() {
		if (msg_queue_id >= 0) {
			return msg_queue_id;
		} else if (msg_queue_id == QUEUE_UNDEF) {
			int flags = IPC_CREAT;
			/**
			* Beskrivelser fra http://publib.boulder.ibm.com/iseries/v5r2/ic2924/index.htm?info/apis/ipcmsggt.htm
			*/
			flags |= S_IRUSR; // 	Permits the creator of the message queue to read it
			flags |= S_IWUSR; // 	Permits the creator of the message queue to write it
			flags |= S_IRGRP; // 	Permits the group associated with the message queue to read it
			flags |= S_IWGRP; // 	Permits the group associated with the message queue to write it
//			cout << "Finner meldingskø med key: "<< msg_key << " flags: " << flags << endl;
			msg_queue_id = msgget(msg_key, flags);
			if (msg_queue_id == -1) {
				cerr << "msgget failed" << strerror(errno) << endl;
			}
			/* else {
				cout << "msgget returned queue_id " << msg_queue_id << endl;
			}*/
		}
		return msg_queue_id;
	}
public:
	/**
	* Instansier med key
	*/
	SysVMsgQueue(key_t key) : msg_key(key), msg_queue_id(QUEUE_UNDEF) {
		
	}
	
	/**
	* Er køen gyldig, får man koblet til spesifisert kø.
	*/
	virtual bool isValid() {
		return getQueueId() >= 0;
	}
	
	/**
	* Motta melding
	*/
	virtual T recvMessage(long msgtype = 0) {
		typename T::msg_type msg_buf;
		int msgflg = 0;
		int read_len = msgrcv(getQueueId(), &msg_buf, sizeof(typename T::msg_cont_type), msgtype, msgflg);
		
		if (read_len != -1) {
			return T(msg_buf, read_len);
		}
	
		cerr << "Fail doing msgrcv: " << strerror(errno) << endl;
		return T();
	}
	
	/**
	* send melding
	*/
	virtual bool sendMessage(T msg) {
		return false;
	}	
};

#endif
