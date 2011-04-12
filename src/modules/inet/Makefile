
CC:=g++
CCARGS:=-Wall -ggdb

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o ../../util/Mutex.o ../../util/FDMonitor.o

all: ineteventlistener.so 

%.o: %.cpp
	$(CC) $(CCARGS) -g -c -o $@ $< 
	
%.cpp: %.h


ineteventlistener.so: InetEventSourceModule.o	InetStreamListener.o InetStreamConnection.o $(MODULE_CORE)
	$(CC) $(CCARGS) -g -fpic -dynamiclib -shared -single_module -lpthread -rdynamic -o $@  $^


clean:
	rm -f *.o *.so
