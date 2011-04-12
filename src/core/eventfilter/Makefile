CC:=g++
CCARGS:=-Wall -ggdb

#
# Alle objekter som skal kompileres i denne katalogen
# 
SRC=EventFilter.o EventFilterHelper.o HasParamFilterRule.o RXParamFilterRule.o MatchParamFilterRule.o

#
# Defaultregelen, alt som havner her skal kompileres
#
all: $(SRC)

#
# alle .cpp filer skal bli objektfiler
#
%.o: %.cpp
	$(CC) $(CCARGS) -g -c -o $@ $<

%.cpp: %.h

#
# Rydderegler
#
clean:
	rm -f *.o
