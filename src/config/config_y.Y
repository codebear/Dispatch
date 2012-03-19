
%{
#include "config_nodes.h"
#include <alloca.h>
namespace dispatch{namespace config{
	class parseDriver;
}}
using namespace ::dispatch::config;
%}

%skeleton "lalr1.cc"

%defines

%locations

%define "namespace" "dispatch"
%define "parser_class_name" "config_parser"

%name-prefix="config::"

%parse-param { class config::parseDriver& driver }

%error-verbose

%union {
	
	// pods
	/**
	* String øverst på stacken
	*/
	char*					str_val;
	
	/**
	* Lang int øverst på stakken
	*/
	long					long_val;
	
	/**
	* Flyttall øverst på stacken
	*/
	double					double_val;

	// struct*s	 
	/**
	* Liste med argumenter
	*/
	config::GConfigArgumentList*		gc_argument_list;
	
	/**
	* Argument til en variabeldeklarasjon
	*/
	config::GConfigArgument* 			gc_argument;
	
	/**
	* Identifier node
	*/
	config::GConfigIdentifier*			gc_identifier;
	
	/**
	* Variabel-deklarasjon med scalar innhold
	*/
	config::GConfigScalarVal*			gc_scalar_val;
	
	/**
	* Blokk
	*/
	config::GConfigBlock*				gc_block;
	
	/**
	* Headeren til en blokk
	*/
	config::GConfigBlockHeader*			gc_block_header;
	
	/**
	* Liste med blokker
	*/
	config::GConfigBlockList*			gc_block_list;
	
	/**
	* Liste med statements
	*/
	config::GConfigStatementList*		gc_statement_list;
	
	/**
	* Generelt statement
	*/
	config::GConfigStatement*			gc_statement;
	
	/**
	* Definerings-statement-node
	*/
	config::GConfigDefineStatement*		gc_define;
	
	/**
	* Variabel-deklarasjonsnode
	*/
	config::GConfigVariableStatement*	gc_variable;
	
	/**
	* Funksjons-deklarasjons-node
	*/
	config::GConfigFunctionStatement*	gc_function;
}


%token	<str_val>		T_LPARAN
%token	<str_val>		T_RPARAN
%token	<str_val>		T_LBRACE
%token	<str_val>		T_RBRACE
%token	<str_val>		T_LBRACKET
%token	<str_val>		T_RBRACKET
%token	<str_val>		T_STRING
%token	<long_val>		T_LONG
%token	<double_val>	T_DOUBLE
%token	<str_val>		T_COMMENT
%token	<str_val>		T_WHITESPACE
%token	<str_val>		T_QUOTED_STRING
%token	<str_val>		T_DOT
%token	<str_val>		T_STMT_TERM
%token	<str_val>		T_ASSIGN
%token	<str_val>		T_COMMA
%token					END	0 "end of file"

%type 	<gc_argument_list>	argument_list
%type 	<gc_argument>		argument
%type	<gc_scalar_val>		scalar_val
%type	<gc_identifier>		identifier
%type	<gc_block>			block
%type	<gc_block_header>	block_start
%type	<gc_block_list>		block_list
%type	<gc_statement_list>	inner_statement_list
%type	<gc_statement>		inner_statement
%type	<gc_variable>		variable_assignment
%type	<gc_define>			define_statement
%type	<gc_function>		function_statement

%initial-action {
// start
	// void
 @$.begin.filename = @$.end.filename = driver.getFileNamePtr(); //new string(driver.getFileName());
/* @$.begin.line = @$.end.line = 0;
 @$.begin.column = @$.end.column = 0;
*/
}

%{
#include "driver.h"

#undef yylex
#define yylex driver.lex
%}

%% /* Grammar rules */

start:	block_list { driver.addBlockList($1); } | END
;

block_list:	block { $$ = new GConfigBlockList($1); }
			| 	block_list block  { $1->appendBlock($2); $$ = $1; }
;

block_start:  	T_STRING  T_LBRACKET identifier T_RBRACKET T_LBRACE { $$ = new GConfigBlockHeader($1, $3, @1); free($1); }
			|	T_STRING  T_LBRACE { $$ = new GConfigBlockHeader($1, @1); free($1); }
;

block:  		block_start inner_statement_list T_RBRACE { $$ = new GConfigBlock($1, $2); }
;


identifier: 	T_STRING { $$ = new GConfigIdentifier($1, @1); free($1); }
			|	identifier T_DOT T_STRING { $1->appendString($3, @3); free($3); $$ = $1; }
;

inner_statement_list:	inner_statement { $$ = new GConfigStatementList($1); }
					|	inner_statement_list	inner_statement { $1->appendStatement($2); $$ = $1; }
;

inner_statement:	block { $$ = $1; }
				|	function_statement { $$ = $1; } 
				| 	variable_assignment { $$ = $1; }
				|	define_statement { $$ = $1; }
;

variable_assignment:	identifier T_ASSIGN argument T_STMT_TERM { $$ = new GConfigVariableStatement($1, $3); }
;

define_statement: identifier T_STMT_TERM { $$ = new GConfigDefineStatement($1); }
;

function_statement:	identifier T_LPARAN argument_list T_RPARAN T_STMT_TERM { $$ = new GConfigFunctionStatement($1, $3); }
;


argument:	scalar_val { $$ = new GConfigArgument($1); }
		|	T_LPARAN argument_list T_RPARAN { $$ = new GConfigArgument($2); }
;

argument_list:	scalar_val { $$ = new GConfigArgumentList($1); }
			|	argument_list T_COMMA scalar_val { $1->appendArgument($3); $$ = $1; }
;

scalar_val: 	T_LONG { $$ = new GConfigScalarVal($1, @1); }
			|	T_DOUBLE { $$ = new GConfigScalarVal($1, @1); }
			|	T_QUOTED_STRING { $$ = new GConfigScalarVal($1, @1); free($1); }
			|	identifier { $$ = new GConfigScalarVal($1); }
;

%%

    void dispatch::config_parser::error (const location_type& loc, const std::string& msg) {
		driver.error(loc, msg);
	}
