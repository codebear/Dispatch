CC:=g++
CFLAGS:=-Wall -ggdb -fPIC

#
# Alle objekter som skal kompileres i denne katalogen
# 
SRC=EventFilter.o EventFilterHelper.o HasParamFilterRule.o RXParamFilterRule.o MatchParamFilterRule.o OriginFilterRule.o

#
# Defaultregelen, alt som havner her skal kompileres
#
all: $(SRC)

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
