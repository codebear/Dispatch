
CC:=g++
CFLAGS:=-Wall -ggdb -fPIC

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o

all: fifoeventlistener.so 

%.o: %.cpp
	$(CC) $(CFLAGS) -g -c -o $@ $< 


fifoeventlistener.so: FifoEventSourceModule.o	FifoStreamListener.o $(MODULE_CORE)
	$(CC) $(CFLAGS) -g -dynamiclib -shared -lpthread -rdynamic -o $@  $^


clean:
	rm -f *.o *.so
