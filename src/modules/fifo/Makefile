
CC:=g++
CCARGS:=-Wall -ggdb

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o

all: fifoeventlistener.so 

%.o: %.cpp
	$(CC) $(CCARGS) -g -c -o $@ $< 


fifoeventlistener.so: FifoEventSourceModule.o	FifoStreamListener.o $(MODULE_CORE)
	$(CC) $(CCARGS) -g -fpic -dynamiclib -shared -single_module -lpthread -rdynamic -o $@  $^


clean:
	rm -f *.o *.so
