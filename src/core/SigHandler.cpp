#include "SigHandler.h"

namespace dispatch { namespace core {

SigHandler SigHandler::_instance;

	void _sig_handler__handle_signals(int sig) {
		SigHandler::instance()->handleSignal(sig);
	}

		SignalRecipient* SigHandler::registerListener(int sig, SignalRecipient* ha) {
			SignalRecipient* old = handlers[sig];
			handlers[sig] = ha;
			if (old == NULL) {
				signal(sig, _sig_handler__handle_signals);
			}
			return old;
		}

		void SigHandler::removeListener(int sig, SignalRecipient* h) {
			handlers[sig] = NULL;
		}
		
		string SigHandler::signalName(int sig) {
			char* str = strsignal(sig);
			if (str != NULL) {
				return string(str);
			}
			return string();
		};

		void SigHandler::handleSignal(int sig) {
			SignalRecipient* old = handlers[sig];
			if (old != NULL) {
				old->handleSignal(sig);
			}
		}
		SigHandler* SigHandler::instance() {
			return &_instance;
		}

}}
