/*
 * DispatchModule.h
 *
 *  Created on: May 20, 2009
 *      Author: bear
 */

#ifndef DISPATCHMODULE_H_
#define DISPATCHMODULE_H_

#include <string>

#include "../config/config.h"
#include "../util/FilteringStreamBuf.h"
#include "../util/NameTimeInserter.h"
#include "../core/QueueProvider.h"
using namespace std;
using namespace dispatch;
using namespace dispatch::util;
using namespace dispatch::config;
namespace dispatch {
namespace module {


/**
* Basisklasse for pluggbare moduler i systemet. Denne er abstrakt (må subklasses).
*/
class DispatchModule : 
	public NameTimeTaggingOutputSet,
	public QueueProvider {
private:
	QueueProvider* queue_provider;
public:

/*	NameTimeTaggedOutput out;
	NameTimeTaggedOutput log;
	NameTimeTaggedOutput err;
	NameTimeTaggedOutput dbg;*/
	
	/**
	* Flagg brukt av system-kjerne-koden for å holde kontroll på om de har startet en modul eller ikke. Primitivt men fungerer :-)
	*/
	bool started;
	
	/**
	* Constructor
	*/
	DispatchModule();
	
	/**
	* Destructor
	*/
	virtual ~DispatchModule();
	/**
	* Skal returnere et beskrivende navn på modulen
	*/
	virtual string getModuleName() = 0;
	
	/**
	* Returnere et tråd-navn, samme som modulnavn
	*/
	string getName();
	
	/**
	* Returnerer true dersom modulen kan brukes som event-kilde
	*/
	virtual bool isEventSource() = 0;
	
	/**
	* Returnerer true dersom modulen kan brukes som event-handler
	*/
	virtual bool isEventHandler() = 0;
	
	/**
	* Fremtidig metode, ikke i bruk enda.
	*/
	//virtual bool isServiceHandler() = 0;
	
	void setQueueProvider(QueueProvider* qp);
	
	QueueProvider* getQueueProvider();
	
	EventQueue* getEventQueue();
	
	/**
	* First-stage oppstart av modulen. Skal finne ut av konfig o.l.
	* Skal returnere true dersom alt er OK, hvis ikke blir ikke modulen startet opp.
	*/
	virtual bool preInitialize() = 0;
	
	/**
	* Start opp modulen til ordinær drift
	*/
	virtual bool startup() = 0;
	
	/**
	* Initierer nedstenging av modulen
	*/
	virtual bool shutdown() = 0;
	
	/**
	* Tilbakekallsmetode som benyttes av visitoren, denne får alle konfigurasjonsfilnodene etter tur.
	*/ 
	virtual bool scanConfigNode(GConfigNode* node) = 0;
	
	/**
	* Hent ut config-visitor. Denne er ikke abstrakt, og trengs ikke implementeres av modulene.
	*/
	virtual config::GConfigNodeVisitor* getConfigVisitor();
};


/**
* Wrapper-visitor som brukes for at en modul skal kunne lese konfigurasjon
*/

class DispatchModuleConfigVisitor : public config::GConfigNodeVisitor {
	DispatchModule* module;
public:
	
	/**
	* Konstructor which takes the module it is going to be used to read config for
	*/
	DispatchModuleConfigVisitor(DispatchModule* mod) : module(mod) {

	}
	/**
	* Node som besøkes
	*/
	virtual void node(config::GConfigNode* node);
};

}} // end namespace 

#endif /* DISPATCHMODULE_H_ */
