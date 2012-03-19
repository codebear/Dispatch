
CC=g++
CFLAGS:=-Wall -ggdb -fPIC

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o

all: inotify.so

%.o: %.cpp
	$(CC) $(CFLAGS) -g -c -o $@ $< 
	
%.cpp: %.h


inotify.so: INotifyModule.o INotifyListener.o $(MODULE_CORE)
	$(CC) $(CFLAGS) -g -dynamiclib -shared -lpthread -rdynamic -o $@  $^


clean:
	rm -f *.o *.so