#include "../../util/Thread.h"
#include <string>
#include <vector>
#include <map>
#include "../../util/NameTimeInserter.h"
#include "../DispatchModule.h"

using namespace std;
using namespace dispatch::util;
namespace dispatch {
namespace module {
namespace inotify {

	/**
	* INotify-overvåker
	*/
	class INotifyListener : public Thread, public NameTimeTaggingOutputSet {
		/**
		* Liste over stiene som skal overvåkes
		*/
		vector<string> watch_paths;
		
		DispatchModule* module;
	protected:
		/**
		* Hjelpemetode for å kaste int til string
		*/
		string i2s(int i);
		
		/**
		* Hjelpemetode for å returnere event-navnene til masken
		*/
		string namesForMask(int mask);
	public:
		INotifyListener(DispatchModule* mod);
		~INotifyListener();
		
		/**
		* Navn for output
		*/
		virtual string getName();
		
		DispatchModule* getModule();
		/**
		* Legg til en sti til overvåkning
		*/
		bool addWatch(string path);
		
		/**
		* Kjør overvåkning i egen tråd
		*/
		void run();
		
		/**
		* Les eventer fra inotify-fd-handle
		* Tar handle som første argument, og en map over hvilke watch-ider som korresponderer til hvilke overvåkede mapper
		*/
		void readEvents(int fd, map<int, string>);
	};
	
}}}
