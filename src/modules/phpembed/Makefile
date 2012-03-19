#/usr/local/include/phpembed

PHP:=/usr/local/include/php
PHPSAPI:=$(PHP)/sapi/embed
PHPZEND:=$(PHP)/Zend
PHPTSRM:=$(PHP)/TSRM
PHPMAIN:=$(PHP)/main
#PHPEMBED_LIB:=/usr/local/lib
PLATFORM_BIT:=`if sh -c "uname -i | grep -q i386"; then echo "32"; else echo "64"; fi`

PHPEMBED:=/home/bear/src/phpembed$(PLATFORM_BIT)/src 
PHPEMBED_LIB:=/home/bear/src/phpembed$(PLATFORM_BIT)/install/lib

CXXFLAGS:= -I $(PHPEMBED) -I $(PHPSAPI) -I $(PHP) -I $(PHPZEND) -I $(PHPTSRM) -I $(PHPMAIN) -Wall -ggdb -fPIC

CXXFLAGSL := -L $(PHPEMBED_LIB) -Wall -ggdb -fPIC

#-I /home/bear/src/phpembed/src -I /usr/local/include/php -I /usr/local/include/php/Zend \
#	    -I /usr/local/include/php/main -I /usr/local/include/php/TSRM



DEPRECATE:= -Wno-deprecated



all: phpembedmodule.so

%.o: %.cpp %.h
	@echo "Compiling with $(DEPRECATE) because php-library uses obsolete header-includes"
	$(CC) $(CXXFLAGS) $(DEPRECATE) -g -c -o $@ $<


phpembedmodule.so: PHPEmbedModule.o PHPEventHandler.o
	$(CC) $(CXXFLAGSL) -g -fpic -dynamiclib -shared -lpthread -rdynamic -lphpembed -lphp5 -o $@ $^


clean:
	rm -f *.so *.o
