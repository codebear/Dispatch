CC:=g++
CCARGS:=-Wall -ggdb

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
	$(CC) $(CCFLAGS) -g -c -o $@ $<

#
# Rydderegler
#
clean:
	rm -f *.o
	$(MAKE) -C eventfilter clean
