PHPEMBED:=/home/bear/src/phpembed/src 
#/usr/local/include/phpembed

PHP:=/usr/local/include/php
PHPSAPI:=$(PHP)/sapi/embed
PHPZEND:=$(PHP)/Zend
PHPTSRM:=$(PHP)/TSRM
PHPMAIN:=$(PHP)/main

CXXFLAGS:= -I $(PHPEMBED) -I $(PHPSAPI) -I $(PHP) -I $(PHPZEND) -I $(PHPTSRM) -I $(PHPMAIN)

#-I /home/bear/src/phpembed/src -I /usr/local/include/php -I /usr/local/include/php/Zend \
#	    -I /usr/local/include/php/main -I /usr/local/include/php/TSRM



DEPRECATE:=
#-Wno-deprecated



all: phpembedmodule.so

%.o: %.cpp %.h
	@echo "Compiling with $(DEPRECATE) because php-library uses obsolete header-includes"
	$(CC) $(CXXFLAGS) $(DEPRECATE) -g -c -o $@ $<


phpembedmodule.so: PHPEmbedModule.o PHPEventHandler.o
	$(CC) -g -fpic -dynamiclib -shared -single_module -lpthread -rdynamic -lphpembed -lphp5 -o $@ $^


clean:
	rm -f *.so *.o
