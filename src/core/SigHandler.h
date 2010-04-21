#ifndef PHPEMBEDMODULE_H_
#define PHPEMBEDMODULE_H_


#include <map>
#include <csignal>
#include <string>

using namespace std;

namespace dispatch { namespace core {
	
	/**
	* Abstract interface for handtering av signal
	*/
	class SignalRecipient {
	public:
		/**
		* Signal mottatt
		*/
		virtual void handleSignal(int signal) = 0;
	};
	
	/**
	* Wrapper-metode som bi kalla
	*/
	void _sig_handler__handle_signals(int sig);
	
	
	/**
	* Handler-klasse for � ta seg av mottatte signaler
	* Subsystema kan be om � f� beskjed fra den her n�r det kjem signala
	*/
	class SigHandler {
		map<int, SignalRecipient*> handlers;
		static SigHandler _instance;
	public:
		/**
		* Hent ut singleton instance av signal-handlern
		*/
		static SigHandler* instance();
		
		/**
		* Motta signaler fra OS-et
		*/
		void handleSignal(int sig);
		
		/**
		* Finn navnet p� et signal
		*/
		static string signalName(int sig);
		
		/**
		* Registrer en listener for et signal
		*/
		SignalRecipient* registerListener(int sig, SignalRecipient* ha);
		
		/**
		* Fjern en listener
		*/
		void removeListener(int sig, SignalRecipient* ha);
	};

}}

#endif


