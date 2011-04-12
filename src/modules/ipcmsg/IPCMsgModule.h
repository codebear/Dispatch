#include "../DispatchModule.h"
#include "MsgListener.h"
#include <vector>


using namespace std;
namespace dispatch { namespace module { namespace ipcmsg {


class _ListenerSpec {
public:
	key_t key;
	int type;
	int count;
	NodeIdent ident;

	_ListenerSpec(key_t k, int t, int c, NodeIdent ident);
};

/**
* Modul for mottak av meldinger via SysV IPC Msg-queue
*/
class IPCMsgModule : public DispatchModule {
	vector<IPCMsgListener*> listeners;
	void initializeListeners(key_t key, long type, int count, NodeIdent id);
	vector<_ListenerSpec*> specs;
public:
	/**
	* Navnet på modulen
	*/
	virtual string getModuleName();
	
	/**
	* Er modulen en event-handler
	*/
	bool isEventHandler();
	
	/**
	* Er modulen en event kilde
	*/
	bool isEventSource();
	
	/**
	* Tidlig initialisering før konfig-håndtering. Hvis modulen vet allerede nå at 
	* den ikke kan, så kan den returnere false, og er ute av spillet for godt.
	*/
	bool preInitialize();
	
	/**
	* Start opp modulen
	*/
	bool startup();
	
	/**
	* Be modulen om å stenge ned etterhvert
	*/
	bool shutdown();
	
	/**
	* Motta en konfigurasjonfilnode for bearbeiding
	*/
	bool scanConfigNode(GConfigNode*);
};


}}} // namespace
