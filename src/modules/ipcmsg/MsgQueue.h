#ifndef _MSG_QUEUE_H
#define _MSG_QUEUE_H 1


/**
* Meldingsk� for mottak av meldinger av type T
*/
template<class T>
class MsgQueue {
public:
	/**
	* Er k�en gyldig
	*/
	virtual bool isValid() = 0;
	
	/**
	* Motta neste melding. Blokkerer sannsynligvis til den ankommer
	*/
	virtual T recvMessage(long t = 0) = 0;
	
	/**
	* Send en melding p� meldingsk�en
	*/
	virtual bool sendMessage(T msg) = 0;
};


#endif
