CC:=g++
CFLAGS:=-Wall -ggdb -fPIC


.PHONY: all fresh clean
all: dispatch modules

fresh: clean all

%.o: %.cpp
	$(CC) $(CFLAGS) -c -g -o $@ $<

%.cpp: %.h %.H

SUBDIRS = config util core modules

dispatch: $(SUBDIRS) dispatch.o
	$(CC) $(CFLAGS) -ldl -lpthread -lboost_regex -g -rdynamic -o dispatch *.o core/*.o core/eventfilter/*.o config/*.o util/*.o modules/*.o  -ldl -lpthread -lboost_regex
	$(MAKE) -C modules dynamic_modules




.PHONY: subdirs $(SUBDIRS) modules clean clean_parser

$(SUBDIRS):
	$(MAKE) -C $@


clean: sub_clean memcheck_clean
	rm -f *.o dispatch 

clean_parser:
	$(MAKE) -C config clean_parser

sub_clean: 
	$(MAKE) -C config clean
	$(MAKE) -C core clean
	$(MAKE) -C modules clean
	$(MAKE) -C util clean

.PHONY: doc doxygen_doc filter_doxygen_errors

doc:	doxygen_doc filter_doxygen_errors

doxygen_doc:
	doxygen Doxyfile

filter_doxygen_errors:
	grep -v BlockingQueue doxygen.error.log | grep -v lex.yy.cc | grep -v config_y.tab.H  | grep -v stack.hh > doxygen.filtered.error.log

.PHONY: memcheck memcheck_clean
memcheck:
	LD_LIBRARY_PATH=/home/bear/src/phpembed32/install/lib valgrind --log-file=leaklog --leak-check=full --show-reachable=yes --track-origins=yes ./dispatch -t
#
#==19392== To see them, rerun with: --leak-check=full --show-reachable=yes
#==19392== Use --track-origins=yes to see where uninitialised values come from

memcheck_clean:
	rm -f leaklog.*

