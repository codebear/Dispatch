CC:=g++
CCARGS:=-Wall -ggdb

all: modules.o DispatchModule.o

dynamic_modules: execute_sub fifo_sub inotify_sub inet_sub ipcmsg_sub

## phpembed_sub 

%.o: %.cpp
	$(CC) $(CCARGS) -c -g -o $@ $<

%.cpp: %.h %.H

phpembed_sub:
	$(MAKE) -C phpembed

inotify_sub:
	$(MAKE) -C inotify

execute_sub:
	$(MAKE) -C execute

fifo_sub:
	$(MAKE) -C fifo

inet_sub:
	$(MAKE) -C inet

ipcmsg_sub:
	$(MAKE) -C ipcmsg

clean:
	rm -f *.o
	$(MAKE) -C phpembed clean
	$(MAKE) -C inotify clean
	$(MAKE) -C fifo clean
	$(MAKE) -C execute clean
	$(MAKE) -C inet clean
	$(MAKE) -C ipcmsg clean
