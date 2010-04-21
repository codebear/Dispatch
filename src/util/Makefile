CC=g++

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
	$(CC) -g -c -o $@ $<

#
# Rydderegler
#
clean:
	rm *.o
