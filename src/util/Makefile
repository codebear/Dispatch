CC:=g++
CFLAGS:=-Wall -ggdb -fPIC

#
# Alle objekter som skal kompileres i denne katalogen
# 
SRC=FileStat.o Thread.o FDMonitor.o Mutex.o NameTimeInserter.o

#
# Defaultregelen, alt som havner her skal kompileres
#
all: $(SRC)

#
# alle .cpp filer skal bli objektfiler
#
%.o: %.cpp
	$(CC) $(CFLAGS) -g -c -o $@ $<

#
# Rydderegler
#
clean:
	rm -f *.o
