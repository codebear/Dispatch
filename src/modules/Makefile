CC:=g++
CFLAGS:=-Wall -ggdb -fPIC

SUBDIRS=execute fifo inotify inet ipcmsg phpembed

all: modules.o DispatchModule.o

.PHONY: $(SUBDIRS) dynamic_modules

dynamic_modules: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@


# dynamic_modules: execute_sub fifo_sub inotify_sub inet_sub ipcmsg_sub phpembed_sub 

%.o: %.cpp
	$(CC) $(CFLAGS) -c -g -o $@ $<

%.cpp: %.h %.H

clean:
	rm -f *.o
	$(MAKE) -C phpembed clean
	$(MAKE) -C inotify clean
	$(MAKE) -C fifo clean
	$(MAKE) -C execute clean
	$(MAKE) -C inet clean
	$(MAKE) -C ipcmsg clean
