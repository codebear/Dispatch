#include "../../util/NameTimeInserter.h"
#include "../../util/Thread.h"
#include "MsgQueue.h"
#include "../../core/StreamEventHandler.h"
#include <sstream>
using namespace dispatch::util;
using namespace dispatch::core;
namespace dispatch { namespace module { namespace ipcmsg {

/**
* Melding mottat p� k�en
*/
class Msg {
	bool valid;
	size_t content_length;
public:
	/**
	* Typedefinisjon for � vite hvor stor plass vi har i meldings-bufferet
	*/
	typedef char msg_cont_type[1024];
	
	/**
	* Meldigs-struct-type-definisjon
	*/
	typedef struct {
		/**
		* Meldingstype for meldingen
		*/
		long type;

		/**
		* Byte-buffer for innholdet
		*/
		msg_cont_type content;
	} msg_type;
	
	/**
	* Buffer for melding
	*/
	msg_type msg;
	
	/**
	* Er meldingen gyldig
	*/
	bool isValid() { return valid; }
	
	/**
	* En melding instansiert med tom-konstruktoren er ikke gyldig
	*/
	Msg() : valid(false) {};
	
	/**
	* Instansier en melding med dataene lest fra ipc-k�en
	*/
	Msg(msg_type m, size_t len) : msg(m), valid(true), content_length(len) {};
	
	/**
	* Les ut meldings-innholdet som et string-objekt
	*/ 
	string getContent() {
		stringstream buf;
		buf.write(msg.content, content_length);
		return buf.str();
	}
};

/**
* Meldings-k�-lytter 
*/
class MsgListener : public NameTimeTaggingOutputSet, public Thread {
	StreamEventHandler handler;
	MsgQueue<Msg>* queue;
	long type;
public:
	/**
	* K�en som skal lyttes fra, og om den skal filtere p� meldingstype
	*/
	MsgListener(MsgQueue<Msg>*queue, long t = 0);

	/**
	* Navn p� tr�den
	*/
	string getName() { return "MsgListener"; }

	/**
	* Start opp
	*/
	void run();
};


}}} // end namespace
