#ifndef PHPEMBEDMODULE_H_
#define PHPEMBEDMODULE_H_

#include "../DispatchModule.h"

using namespace dispatch::config;
using namespace dispatch::config::filter;


namespace dispatch {
namespace module {
namespace phpembed {

/**
* Modul-implementasjon som skal kunne h�ndtere eventer ved hjelp av PHP-skript
*/	
class PHPEmbedModule : public DispatchModule
{
public:
	PHPEmbedModule();
	virtual ~PHPEmbedModule();
	
	/**
	* Er ikke en event-kilde
	*/
	virtual bool isEventSource() {
		return false;
	}
	
	/**
	* Er en event-handler
	*/
	virtual bool isEventHandler() {
		return true;
	}
	
	/**
	* Er ikke en service-handler
	* (Dette er noe som er med som en ide til fremtidige versjoner. Er ikke i bruk enn s� lenge.
	*/
	virtual bool isServiceHandler() {
		return false;	
	}
	
	/**
	* Navnet p� modulen. Dette navnet er det som skal henvises til i konfigurasjonsfilene
	*/
	virtual string getModuleName() {
		return string("PHPEmbed");	
	}
	
	/**
	* F�rste-trinns initialisering
	*/ 

	virtual bool preInitialize();

	/**
	* Start opp tjenester ihht til konfigurasjon
	*/
	virtual bool startup();
	
	/**
	* Steng ned
	*/
	virtual bool shutdown();
	
	/**
	* Kalles av konfigurasjons-apiet n�r det blir funnet konfigurasjonsnoder som har med denne modulen � gj�re.
	*/
	virtual bool scanConfigNode(GConfigNode* node);
};

}}}
#endif /*PHPEMBEDMODULE_H_*/
