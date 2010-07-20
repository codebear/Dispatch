#
#
#
CC:=g++

LEXER:=lex.yy.cc

#
# Alle objektfilene som skal kompileres i denne katalogen
#
all: config.o driver.o config_nodes.o config_y.tab.o lex.yy.o node_list.o node_base.o


#
# Regel som matcher alle objectfiler, og som sier at hver objektfil skal bestå av alle .C og .cc filer med samme navn
#
%.o: %.C %.cc %.cpp
	$(CC) -g -c -o $@ $<

#
# Dependecier videre dersom tilhørende header-filene er endret
#
%.C: %.h %.H
%.cc: %.h %.H
%.cpp: %.h %.H

#
# Spesielle avhengigheter der navnene ikke matcher
#
config.cpp: config_y.tab.C

#config_nodes.C: config_nodes.h

#
# Av yacc-fila skal det generes kildekode med bison
#
config_y.tab.C: config_y.Y
	bison -v -l -g -d -k config_y.Y


#
# Av lex-fila skal det genereres kildekode med flex
#
$(LEXER): config_l.l
	flex -+ --header-file=config_l.h config_l.l

parser: config_y.tab.C $(LEXER)

#
# Rydde-regler
#
clean:
	rm *.o  || true
	@echo "run [make clean_parser] in the config-dir for generating a new parser (requires bison and flex)"

clean_parser: clean_bison clean_flex

clean_bison:
	rm stack.hh location.hh position.hh config_y.tab.C config_y.tab.H config_y.output config_y.dot || true

clean_flex:
	rm lex.yy.cc config_l.h || true
