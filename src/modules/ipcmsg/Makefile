
CC=g++

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o ../../util/Mutex.o ../../util/FDMonitor.o

all: ipcmsglistener.so

%.o: %.cpp
	$(CC) -g -c -o $@ $< 


ipcmsglistener.so: IPCMsgModule.o MsgListener.o $(MODULE_CORE)
	$(CC) -g -fpic -dynamiclib -shared -single_module -lpthread -rdynamic -o $@  $^


clean:
	rm -f *.o *.so
