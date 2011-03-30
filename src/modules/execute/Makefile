
CC=g++

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o

all: executecommandmodule.so

%.o: %.cpp %.h
	$(CC) -g -c -o $@ $< 


executecommandmodule.so: ExecuteCommandModule.o ExecuteCommandHandler.o  $(MODULE_CORE)
	$(CC) -g -fpic -dynamiclib -shared -single_module -lpthread -rdynamic -o $@  $^

clean:
	rm -f *.o *.so
