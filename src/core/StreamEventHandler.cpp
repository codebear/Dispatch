#include "StreamEventHandler.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include "../util/StringHelper.h"

using namespace std;

namespace dispatch { namespace core {

void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ",
                      int max_cnt = 0)
{
//	cout << "in Tokenize" << endl;
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
//    cout << "lastPos" << lastPos << endl;
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    if (max_cnt == 1) {
    	tokens.push_back(str);
    	return;
    }
/*    if (max_cnt) {
    	max_cnt;
    }*/
//    cout << "max_cnt" << max_cnt << endl;

    int cnt = 0;
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
//        cout << "run substr str: " << str <<" lastPos: " << lastPos << " pos: " <<pos << endl;
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        if (max_cnt && (++cnt >= max_cnt-1)) {
//	        cout << "run substr str: " << str <<" lastPos: " << lastPos << endl;
        	tokens.push_back(str.substr(lastPos));
        	return;
        }

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


StreamEventHandler::StreamEventHandler() :
	queue(NULL),
	working_event(NULL)
{
}

StreamEventHandler::StreamEventHandler(EventQueue* ev) : 
	queue(ev),
	working_event(NULL) {

}

StreamEventHandler::StreamEventHandler(EventQueue* ev, config::NodeIdent& id) :
	queue(ev),
	working_event(NULL),
	base_ident(id)
{
	
}

void StreamEventHandler::setEventQueue(EventQueue* e) {
	queue = e;
}

EventQueue* StreamEventHandler::getEventQueue() {
	return queue;
}

void StreamEventHandler::handleStreamChunk(string chunk, bool complete) {
	stringstream bygger;
	bygger << rest_buffer;
	bygger << chunk;
	vector<string> linjer;
	string buf = bygger.str();
	do {
		size_t pos = buf.find_first_of("\n");
		if ((pos == string::npos) && !complete) {
			/**
			* No match
			* Ta vare på resten og avslutt
			*/
			rest_buffer = buf;
			return;
		}
		string line;
		/**
		* Hvis vi fant et linjeskift så tar vi rede på det.
		*/
		if (pos != string::npos) {
			line = buf.substr(0, pos);
			/**
			* Buffer får inneholde resten av chunken
			*/
			buf = buf.substr(pos+1);
//			buf = buf.substr(pos+1);
		} else {
			/**
			* Hvis vi ikke fant linjeskift, og vi har havnet her, betyr det
			* at complete er sann. Resten av buf er dermed siste linje
			*/
			line = buf;
			buf = "";
		}
		cout << "HAndler line: " << line << endl;
		handleStreamLine(line);
		
	} while(buf.length());
	if (complete) {
		handleStreamLine(buf, complete);
		buf = "";
	}
	rest_buffer = buf;
	
/*	Tokenize(buf, linjer, "\n");
	for (int i = 0; i < linjer.size(); i++) {
		cout << "Linje " << i << ": " << linjer[i] << endl;
	}
*/
}

/**
* Hele bufferen blir parset som en event, også om det inneholder doble linjeskift
*/
void StreamEventHandler::handleCompleteEvent(string event) {
	vector<string> linjer;
	Tokenize(event, linjer, "\n");
	// Flush eventuelt noe som ligger der
	handleStreamLine("", true);
	for (uint i = 0; i < linjer.size(); i++) {
		cout << "Linje " << i << ": " << linjer[i] << endl;
		handleStreamLine(linjer[i]);
	}
	// Avslutt
	handleStreamLine("", true);
}

void StreamEventHandler::handleStreamLine(string line, bool complete) {
	string::iterator it = line.end();
	size_t last_sensible = line.find_last_not_of("\r\n");
	if (last_sensible != string::npos) {
		line = line.substr(0, last_sensible+1);
	} else if (line.find_first_of("\r\n") != string::npos) {
		/**
		* Hvis forrige søk ikke gav noen treff på noe annet en \r\n, og vi får treff på en \r\n nå,
		* så må vel det bety at linja er tom?
		*/
		line = "";
	}
	cerr << "line " << line.length()<<" [" << line << "]" << endl;
	if (!line.length()) {
		/**
    	* If event is not empty, then we're done with it, so ship it off
	 	*/
		if (working_event && !working_event->isEmpty()) {
   			getEventQueue()->queue(working_event);
   			/**
   			* Allocate new event for next round
   			*/
 			working_event = NULL;
		}
		return;
	}
	
	if (!working_event) {
		working_event = new Event();
	}
	
	vector<string> line_parts;
	
	try {
		Tokenize(line, line_parts, ":", 2);
	} catch(std::out_of_range &ex) {
		// void
		cerr << "Exception under parsing av event-linje" << endl;	
	}
	
	if (line_parts.size() == 2) {
		/**
    	* Linja e rett formatert
		*/
		cout << "Setter parametre [" << line_parts[0] << "]:[" << line_parts[1] << "]" <<endl;
		string key = string_trim(line_parts[0]);
		string value = string_trim(line_parts[1]);
		working_event->setParameter(key, value);
	} else {
		/**
		* Linja e skeivt formatert. What to do?
		*/
		cerr << "Bad event line: [" << line << "]" << endl;
	}
	
	if (complete) {
		if (!working_event->isEmpty()) {
			getEventQueue()->queue(working_event);
		}
		working_event = NULL;
	}
}

}} // end namespace
