CC:=g++
CFLAGS:=-Wall -ggdb -fPIC

#
# Alle objekter som skal kompileres i denne katalogen
# 
SRC=EventQueue.o StreamEventListener.o DispatchMain.o SigHandler.o StreamEventHandler.o

#
# Defaultregelen, alt som havner her skal kompileres
#
all: $(SRC)
	$(MAKE) -C eventfilter

#
# alle .cpp filer skal bli objektfiler
#
%.o: %.cpp
	$(CC) $(CFLAGS) -g -c -o $@ $<

%.cpp: %.h
#
# Rydderegler
#
clean:
	rm -f *.o
	$(MAKE) -C eventfilter clean
