/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *********************************************
 */

/* -------------------------------------------------

            CFG for tinyL LANGUAGE

     PROGRAM ::= STMTLIST .
     STMTLIST ::= STMT MORESTMTS
     MORESTMTS ::= ; STMTLIST | epsilon
     STMT ::= ASSIGN | READ | PRINT
     ASSIGN ::= VARIABLE = EXPR
     READ ::= ? VARIABLE
     PRINT ::= ! VARIABLE
     EXPR ::= + EXPR EXPR |
              - EXPR EXPR |
              * EXPR EXPR |
              VARIABLE | 
              DIGIT
     VARIABLE ::= a | b | c | d | e 
     DIGIT ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

     NOTE: tokens are exactly a single character long

     Example expressions:

           +12.
           +1b.
           +*34-78.
           -*+1+2a58.

     Example programs;

         ?a;?b;c=+3*ab;d=+c1;!d.
         b=-*+1+2a58;!b.

 ---------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Instr.h"
#include "InstrUtils.h"
#include "Utils.h"

#define MAX_BUFFER_SIZE 500
#define EMPTY_FIELD 0xFFFFF
#define token *buffer

/* GLOBALS */
static char *buffer = NULL;	/* read buffer */
static int regnum = 1;		/* for next free virtual register number */
static FILE *outfile = NULL;	/* output of code generation */

/* Utilities */
static void CodeGen(OpCode opcode, int field1, int field2, int field3);
static inline void next_token();
static inline int next_register();
static inline int is_digit(char c);
static inline int to_digit(char c);
static inline int is_identifier(char c);
static char *read_input(FILE * f);

/* Routines for recursive descending parser LL(1) */
static void program();
static void stmtlist();
static void morestmts();
static void stmt();
static void assign();
static void read();
static void print();
static int expr();
static int variable();
static int digit();

/*************************************************************************/
/* Definitions for recursive descending parser LL(1)                     */
/*************************************************************************/
static int digit()
{
	int reg;

	if (!is_digit(token)) {
		ERROR("Expected digit\n");
		exit(EXIT_FAILURE);
	}
	reg = next_register();
	CodeGen(LOADI, reg, to_digit(token), EMPTY_FIELD);
	next_token();
	return reg;
}

static int variable()  /* only called for R-values */
{
        int reg;
		/* YOUR CODE GOES HERE */
        if((token == 'a') | (token == 'b') | (token == 'c') | (token == 'd') | (token == 'e')){
            reg = next_register();
            CodeGen(LOADI, reg, token, EMPTY_FIELD);
            next_token();
            return reg;

        } else {
            ERROR("Expected correct variable\n");
            exit(EXIT_FAILURE);
        }
}

static int expr()
{
	int reg, left_reg, right_reg;

	switch (token) {
	case '+': // DONE
		next_token();
		left_reg = expr();
		right_reg = expr();
		reg = next_register();
		CodeGen(ADD, reg, left_reg, right_reg);
		return reg;
		/* YOUR CODE GOES HERE */
    case '-': // DONE
        next_token();
        left_reg = expr();
        right_reg = expr();
        reg = next_register();
        CodeGen(SUB, reg, left_reg, right_reg);
        return reg;
	case '*': // DONE
        next_token();
        left_reg = expr();
        right_reg = expr();
        reg = next_register();
        CodeGen(MUL, reg, left_reg, right_reg);
        return reg;
    case '0':
        return digit();
	case '1':
        return digit();
	case '2':
        return digit();
	case '3':
        return digit();
	case '4':
        return digit();
	case '5':
        return digit();
	case '6':
        return digit();
	case '7':
        return digit();
	case '8':
        return digit();
	case '9':
		return digit();
    case 'a':
        return variable();
    case 'b':
        return variable();
    case 'c':
        return variable();
    case 'd':
        return variable();
    case 'e':
        return variable();
	default:
		ERROR("Symbol %c unknown\n", token);
		exit(EXIT_FAILURE);
	}
}

static void assign()
{
	char ident; // identifier - names of variables, functions, etc...)
        int reg;
    //printf("\nAssign first token = %c\n", token);
	if (!is_identifier(token)) {
        // if the token is not an identifier, then exit assign
		ERROR("Expected identifier (assign method)\n");
		exit(EXIT_FAILURE);
	}
	ident = token; // set the identifier equal to the token
	next_token(); // get the next token (which should be equals)
	if (token != '=') {
        // if the token after the identifier is not equals, exit
		ERROR("Expected equal sign for assignment statement\n");
		exit(EXIT_FAILURE);
	};
    // get the next token
	next_token();
    // either a digit or a variable.
        reg = expr();
	
    // write out 'STORE <identifier> <register>     i.e. STORE a r5 
	CodeGen(STORE, ident, reg, EMPTY_FIELD);
}

static void read()
{
	/* YOUR CODE GOES HERE */
    if (token != '?') {
        ERROR("Expected read statement");
        exit(EXIT_FAILURE);
    }
    // get the variable the input is going to be stored in
    next_token();
    // make sure this token is an identifier
    if(!is_identifier(token)) {
        ERROR("Expected identifier (read method)\n");
        exit("EXIT_FAILURE");
    }
    char ident = token;
    CodeGen(READ, ident, EMPTY_FIELD, EMPTY_FIELD);
    // add support to now store this variable that is read

    

    // finish, move on to next token
    next_token();
}

static void print()  /* variables are handled explicitly without recursive call */
{
	if (token != '!') {
		ERROR("Expected print statement\n");
		exit(EXIT_FAILURE);
	}
	next_token();
	if (!is_identifier(token)) {
		ERROR("Expected identifier (print method)\n");
		exit(EXIT_FAILURE);
	}
	CodeGen(WRITE, token, EMPTY_FIELD, EMPTY_FIELD);
	next_token();
}

static void stmt()
{
	/* YOUR CODE GOES HERE */
    switch(token){
        case 'a':
            return assign();
        case 'b':
            return assign();
        case 'c':
            return assign();
        case 'd':
            return assign();
        case 'e':
            return assign();
        case '?':
            return read();
        case '!':
            return print();
    }
}

static void morestmts()
{
	/* YOUR CODE GOES HERE */
    if (token == ';'){
        //printf("\nMore Statements Function\n");
        next_token();
        return stmtlist();
    } else {
        return;
    }
}

static void stmtlist()
{
	/* YOUR CODE GOES HERE */
    //printf("\nHIT, token = %c\n", token);
    switch(token){
    case 'a': 
        stmt();
        return morestmts();
    case 'b':
        stmt();
        morestmts();
    case 'c':
        stmt();
        morestmts();
    case 'd':
        stmt();
        morestmts();
    case 'e':
        stmt();
        morestmts();
    case '?': 
        stmt();
        morestmts();
    case '!':
        stmt();
        return morestmts();
    default:
        ERROR("Symbol %c unknown\n", token);
        exit(EXIT_FAILURE);
    }
}

static void program()
{
        //assign();     /* DUMMY - NEEDS TO BE DELETED */
        //next_token(); /* DUMMY - NEEDS TO BE DELETED */
        //print();      /* DUMMY - NEEDS TO BE DELETED */

	/* YOUR CODE GOES HERE */

    switch(token){
    case 'a': 
        return stmtlist();
    case 'b':
        return stmtlist();
    case 'c':
        return stmtlist();
    case 'd':
        return stmtlist();
    case 'e':
        return stmtlist();
    case '?': 
        return stmtlist();
    case '!':
        return stmtlist();
    default:
        ERROR("Symbo %c unknown\n", token);
        exit(EXIT_FAILURE);

    }


	if (token != '.') {
		ERROR("Program error.  Current input symbol is %c\n", token);
		exit(EXIT_FAILURE);
	};
    //print();
}

/*************************************************************************/
/* Utility definitions                                                   */
/*************************************************************************/
static void CodeGen(OpCode opcode, int field1, int field2, int field3)
{
	Instruction instr;

	if (!outfile) {
		ERROR("File error\n");
		exit(EXIT_FAILURE);
	}
	instr.opcode = opcode;
	instr.field1 = field1;
	instr.field2 = field2;
	instr.field3 = field3;
	PrintInstruction(outfile, &instr);
}

static inline void next_token()
{
	if (*buffer == '\0') {
		ERROR("End of program input\n");
		exit(EXIT_FAILURE);
	}
	printf("%c ", *buffer);
	if (*buffer == ';')
		printf("\n");
	buffer++;
	if (*buffer == '\0') {
		ERROR("End of program input\n");
		exit(EXIT_FAILURE);
	}
	if (*buffer == '.')
		printf(".\n");
}

static inline int next_register()
{
	return regnum++;
}

static inline int is_digit(char c)
{
	if (c >= '0' && c <= '9')
		return 1;
	return 0;
}

static inline int to_digit(char c)
{
	if (is_digit(c))
		return c - '0';
	WARNING("Non-digit passed to %s, returning zero\n", __func__);
	return 0;
}

static inline int is_identifier(char c)
{
	if (c >= 'a' && c <= 'e')
		return 1;
	return 0;
}

static char *read_input(FILE * f)
{
	size_t size, i;
	char *b;
	int c;

	for (b = NULL, size = 0, i = 0;;) {
		if (i >= size) {
			size = (size == 0) ? MAX_BUFFER_SIZE : size * 2;
			b = (char *)realloc(b, size * sizeof(char));
			if (!b) {
				ERROR("Realloc failed\n");
				exit(EXIT_FAILURE);
			}
		}
		c = fgetc(f);
		if (EOF == c) {
			b[i] = '\0';
			break;
		}
		if (isspace(c))
			continue;
		b[i] = c;
		i++;
	}
	return b;
}

/*************************************************************************/
/* Main function                                                         */
/*************************************************************************/

int main(int argc, char *argv[])
{
	const char *outfilename = "tinyL.out";
	char *input;
	FILE *infile;

	printf("------------------------------------------------\n");
	printf("CS314 compiler for tinyL\n");
	printf("------------------------------------------------\n");
	if (argc != 2) {
		ERROR("Use of command:\n  compile <tinyL file>\n");
		exit(EXIT_FAILURE);
	}
	infile = fopen(argv[1], "r");
	if (!infile) {
		ERROR("Cannot open input file \"%s\"\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	outfile = fopen(outfilename, "w");
	if (!outfile) {
		ERROR("Cannot open output file \"%s\"\n", outfilename);
		exit(EXIT_FAILURE);
	}
	input = read_input(infile);
	buffer = input;
	program();
	printf("\nCode written to file \"%s\".\n\n", outfilename);
	free(input);
	fclose(infile);
	fclose(outfile);
	return EXIT_SUCCESS;
}
