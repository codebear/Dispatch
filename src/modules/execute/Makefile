
CC=g++

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o

all: executecommandhandler.so

%.o: %.cpp
	$(CC) -g -c -o $@ $< 


executecommandhandler.so: ExecuteCommandHandler.o  $(MODULE_CORE)
	$(CC) -g -fpic -dynamiclib -shared -single_module -lpthread -rdynamic -o $@  $^

clean:
	rm *.o *.so
