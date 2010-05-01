CC=g++

#
# Alle objekter som skal kompileres i denne katalogen
# 
SRC=EventFilter.o HasParamFilterRule.o

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
