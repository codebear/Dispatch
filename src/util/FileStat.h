/*
 * FileStat.h
 *
 *  Created on: May 24, 2009
 *      Author: bear
 */

#ifndef FILESTAT_H_
#define FILESTAT_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <string>
using namespace std;

namespace dispatch { namespace util {
/**
* Liten utility-klasse for å statte en filsystem-node
*/
class FileStat {
	/**
	* sti til noden som skal stattes
	*/ 
	string file;
	/**
	* Resultatet av staten
	*/
	struct stat64 file_stat;
	/**
	* Flag som sier som man har kjørt stat
	*/
	bool statted;
	/**
	* Intern metode som kjører den faktiske statten
	*/
	inline void _stat();
public:
	
	/**
	* Tom konstruktør
	*/ 
	FileStat();
	/**
	* Konstruktør med navn på fila som skal sjekkes
	*/
	FileStat(string file);
	virtual ~FileStat();
	/**
	* Returnere størrelsen på fila
	*/
	long fileSize();
	
	/**
	* Er dette en katalog
	*/
	bool isDir();
	
	/**
	* Er dette en fil?
	*/
	bool isFile();
	
	/**
	* Er dette en fifo?
	*/
	bool isFifo();
	
};
}}
#endif /* FILESTAT_H_ */
