/*
 * FileStat.cpp
 *
 *  Created on: May 24, 2009
 *      Author: bear
 */

#include "FileStat.h"


namespace dispatch {namespace util {

FileStat::FileStat() {
	statted = false;
}

FileStat::~FileStat() {
	// TODO Auto-generated destructor stub
}

FileStat::FileStat(string f) : file(f) {
	statted = false;
}

void FileStat::_stat() {
	if (statted) return;
	stat64(file.c_str(), &file_stat);
	statted = true;
}

long FileStat::fileSize() {
	_stat();
	return file_stat.st_size;
}

bool FileStat::isDir() {
	_stat();
	return (file_stat.st_mode & S_IFDIR);
}

bool FileStat::isFifo() {
	_stat();
	return (file_stat.st_mode & S_IFIFO);
}

bool FileStat::isFile() {
	_stat();
	return (file_stat.st_mode & S_IFREG);
}

}} // end namespace
