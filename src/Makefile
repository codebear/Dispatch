CC=g++


all: config_sub core_sub modules_sub util_sub dispatch

%.o: %.cpp
	$(CC) -c -g -o $@ $<

%.cpp: %.h %.H
 
dispatch: dispatch.o core config util
	g++ -g -rdynamic -o dispatch -lpthread -ldl *.o core/*.o config/*.o util/*.o modules/*.o 

config_sub:
	$(MAKE) -C config

util_sub:
	$(MAKE) -C util

core_sub:
	$(MAKE) -C core

modules_sub:
	$(MAKE) -C modules


clean: sub_clean memcheck_clean
	rm *.o dispatch 

sub_clean: 
	$(MAKE) -C config clean
	$(MAKE) -C core clean
	$(MAKE) -C modules clean
	$(MAKE) -C util clean

doc:	doxygen_doc filter_doxygen_errors

doxygen_doc:
	doxygen Doxyfile

filter_doxygen_errors:
	grep -v BlockingQueue doxygen.error.log | grep -v lex.yy.cc | grep -v config_y.tab.H  | grep -v stack.hh > doxygen.filtered.error.log

memcheck:
	valgrind --log-file=leaklog --leak-check=full ./dispatch

memcheck_clean:
	rm leaklog.*

