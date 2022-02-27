/*************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Winter, 2021
*************************************************************
* File name: parser.h
* Compiler: MS Visual Studio 2019
* Author: Svillen Ranev - Paulo Sousa - Abdulah
* Course: CST 8152 – Compilers, Lab Section: [014]
* Assignment: A3.
* Date: Sep 01 2020
* Purpose: This file is the main header for Parser (.h)
*************************************************************/

/* TODO_101: Adjust the function header */

/* Inclusion section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "token.h"
#include "buffer.h"

/* Global vars */
static Token lookahead;
int syntaxErrorNumber = 0;
extern bStructure* stringLiteralTable;
extern int line;
extern Token tokenizer();
extern char* keywordTable[];

/* Mock code: remove this UNKNOWN when variables defined */
#define UNKNOWN 0

/* TODO_102: Create ALL constants for keywords (sequence given in table.h) */
/* Constants */
#define	NO_ATTR	-1
#define MAIN 	0
#define IF		1
#define THEN	2
#define	ELSE	3
#define WHILE	4
#define DO		5
#define READ	6
#define WRITE	7
#define TRUE	8
#define FALSE	9
// Continue the code

/* Function definitions */
void startParser(void);
void matchToken(int, int);
void syncErrorHandler(int);
void printError();
//void printMessage(char*);

/* TODO_103: Place ALL non-terminal function declarations */
void program(void);
void optionalStatements(void);
void statements(void);
void statementsPrime(void);
void statement(void);
void assignmentStatement(void);
void assignmentExpression(void);
void selectionStatement(void);
void iterationStatement(void);
void preCondition(void);
void inputStatement(void);
void variableList(void);
void variableListPrime(void);
void variableIdentifier(void);
void outputStatement(void);
void outputStatementPrime(void);
void optVariableList(void);
void arithmeticExpression(void);
void unaryArithmeticExpression(void);
void additiveArithmaticExpression(void);
void additiveArithmaticExpressionPrime(void);
void multiplicativeArithmaticExpression(void);
void multiplicativeArithmaticExpressionPrime(void);
void primaryArithmaticExpression(void);
void stringExpression(void);
void stringExpressionPrime(void);
void primaryStringExpression(void);
void conditionalExpression(void);
void logicalOrExpression(void);
void logicalOrExpressionPrime(void);
void logicalAndExpression(void);
void logicalAndExpressionPrime(void);
void logicalNotExpression(void);
void relationalExpression(void);
void relationalAExpression(void);
void relationalAExpressionPrime(void);
void primaryARelationalExpression(void);
void relationalSExpression(void);
void relationalSExpressionPrime(void);
void primarySRelationalExpression(void);


// Continue the code
