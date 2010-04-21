#ifndef _FILTERING_STREAM_BUF_H
#define _FILTERING_STREAM_BUF_H 1

#include <iostream>

/*
* Inspirert fra
* http://kanze.james.neuf.fr/articles/fltrsbf1.html
*/
using namespace std;
namespace dispatch { namespace util {

/**
* Spesiell stream-buffer som modifisere dataene som går gjennom.
* Inserter_T klassen kan sette inn data på begynnelsen av hver linje
*/
template< class Inserter_T >
class FilteringOutputStreambuf : public streambuf
{

public:
	/**
	* Konstruktor med Inserter_T instans
	*/
	FilteringOutputStreambuf(
		streambuf*     dest,
		Inserter_T     i,
		bool           deleteWhenFinished  = false ) : 
			myDest(dest),
			myInserter(i),
			myDeleteWhenFinished(deleteWhenFinished) {
	
	}
			
	/**
	* Konstruktor uten Inserter_T instans
	*/
	FilteringOutputStreambuf(
		streambuf*     dest ,
		bool           deleteWhenFinished = false ) :
			myDest(dest),
			myDeleteWhenFinished(deleteWhenFinished) {
	}
	
	virtual ~FilteringOutputStreambuf() {
	
	}
	
	/**
	* Blir kallt på begynnelsen av hver linje
	*/
	virtual int overflow( int ch )   {
		int result( EOF ) ;
		if ( ch == EOF )
			result = sync() ;
		else if ( myDest != NULL ) {
//			assert( ch >= 0 && ch <= UCHAR_MAX ) ;
			result = myInserter( *myDest , ch ) ;
		}
		return result ;
	}
	
	/**
	* Denne skal ikke gjøre noe på denne streamen
	*/
	virtual int underflow() { 
		return EOF;
	}
	
	/**
	* Denne sender videre sync-kall til den egentlige strømmen
	*/
	virtual int sync() {
		int	result( EOF ) ;
		if ( myDest != NULL ) {
			result = myDest->pubsync() ;
		}
		return result ;   	                     
	}
	
	/**
	* Sett buffer-egenskaper
	*/
	virtual streambuf*  setbuf( char* p , std::streamsize len ) {
		return myDest == NULL ? 
			static_cast<std::streambuf*>(NULL) :
			myDest->pubsetbuf(p, len);
	}

	/**
	* Hent ut den aktive inserter objektet
	*/
	inline Inserter_T&    inserter() {
		return myInserter;
	}

private:
	streambuf*          myDest ;
	Inserter_T          myInserter ;
	bool                myDeleteWhenFinished ;
};


/**
* Stream-formater som setter inn timestamp og et navn først på alle linjene, egnet for logging
*/
class TimeStampInserter
{
public:

	/**
	* Prefix-string
	*/
	TimeStampInserter(string pref) : prefix(pref), myAtStartOfLine(true) {
    	
	}
	
	TimeStampInserter()	: myAtStartOfLine( true ) {
	
	}

	/**
	* Set en string som skal skrives ut før timestamp
	*/
	virtual void setPrefix(string s) {
		prefix = s;
	}
		
	/**
	* Hent ut en string som skal hentes ut før timestamp
	*/
	virtual string getPrefix() {
		return prefix;
	}
		
	
	/**
	* Sett en string som skal skrives ut etter timestamp
	*/
	virtual void setPostfix(string s) {
		postfix = s;
	}


	/**
	* Hent ut stringen som skal skrives etter timestamp
	*/
	virtual string getPostfix() {
		return postfix;
	}

	/**
	* functor-impolementasjon
	*/
	int operator()( streambuf& dst , int ch )
	{
		bool                errorSeen( false ) ;
		if ( myAtStartOfLine && ch != '\n' )
		{
			int length;
		
			string pref = getPrefix();
		    length = pref.length();
		    if (length) {
		    	if (dst.sputn(pref.c_str(), length) != length) {
		    		errorSeen = true;
		    	}
		    }
			
		    time_t              t( time( NULL ) ) ;
		    tm*                 time = localtime( &t ) ;
		    char                buffer[ 128 ] ;
		    
		    length =    strftime( buffer ,
		                  sizeof( buffer ) ,
		                  "%c" ,
		                  time ) ;
//                assert( length > 0 ) ;
			if ( dst.sputn( buffer , length ) != length ) {
			    errorSeen = true ;
			}

			string postf = getPostfix();
			length = postf.length();
			if (length) {
				if (dst.sputn(postf.c_str(), length) != length) {
					errorSeen = true;
				}
			}

		}
		myAtStartOfLine = (ch == '\n') ;
		return errorSeen
		    ?   EOF 
		    :   dst.sputc( ch ) ;
	}

private:
	bool                 myAtStartOfLine ;
	string prefix;
	string postfix;
};

}}  // end namespace
#endif
