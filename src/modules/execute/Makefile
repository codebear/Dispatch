
CC:=g++
CFLAGS:=-Wall -ggdb -fPIC

MODULE_CORE=../DispatchModule.o ../../util/Thread.o ../../core/StreamEventListener.o ../../core/EventQueue.o ../../util/FileStat.o

all: executecommandmodule.so

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -g -c -o $@ $< 


executecommandmodule.so: ExecuteCommandModule.o ExecuteCommandHandler.o  $(MODULE_CORE)
	$(CC) $(CFLAGS) -g -dynamiclib -shared -lpthread -rdynamic -o $@  $^

clean:
	rm -f *.o *.so
