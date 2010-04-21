
CC=g++

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o

all: inotify.so

%.o: %.cpp %.h
	$(CC) -g -c -o $@ $< 


inotify.so: INotifyModule.o INotifyListener.o $(MODULE_CORE)
	$(CC) -g -fpic -dynamiclib -shared -single_module -lpthread -rdynamic -o $@  $^


clean:
	rm *.o *.so