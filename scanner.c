/********************************************************************
 * Filename:	scanner.c
 * Authors:		Justin Fortin, Kalen Gladu-Lauridsen
 * Date:		2021-03-20
 * PURPOSE:
 *    Functions implementing a Lexical Analyzer (Scanner)
 *    as required for CST8152, Assignment #2
 *    The file is COMPLETE;
 *    Created in part by: Svillen Ranev - Paulo Sousa - Abdullah Kadri
 *    Version: 1.21
 *    Date: 1 Jan 2021
 *******************************************************************
 */


 /* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
  * to suppress the warnings about using "unsafe" functions like fopen()
  * and standard sting library functions defined in string.h.
  * The define does not have any effect in Borland compiler projects.
  */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */
#include < ctype.h >
  /*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern bPointer stringLiteralTable;		/* String literal table */
int line;								/* current line number of the source code */
extern int errorNumber;					/* defined in platy_st.c - run-time error number */

static char debugMode = 0;				/* optional for debugging */

/* Local(file) global objects - variables */
static bPointer lexemeBuffer;			/* pointer to temporary lexeme buffer */
static bPointer sourceBuffer;			/* pointer to input source buffer */
/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */
static int nextClass(char c);			/* character class function */
static int nextState(int, char);		/* state machine function */
static int isKeyword(char kw_lexeme[]);	/* keywords lookup function */


/*************************************************************
 * Purpose:				Intitializes scanner
 * Version:				1.0
 * Paramaters:			Pointer to Buffer struct bPointer (bPointer psc_buf)
 * Functions Called:	bIsEmpty(), bRewind(), bClean()
 * Return Value:		int
 ************************************************************/
int startScanner(bPointer psc_buf) {
	if (bIsEmpty(psc_buf))
		return EXIT_FAILURE; /*1*/
	/* in case the buffer has been read previously  */
	bRewind(psc_buf);
	bClean(stringLiteralTable);
	line = 1;
	sourceBuffer = psc_buf;
	return EXIT_SUCCESS; /*0*/
}


/*************************************************************
 * Purpose:				Reads through the lexeme from the buffer then finds and returns the matching token
 * Version:				1.0
 * Paramaters:			N/A
 * Functions Called:	bGetCh(), isspace(), bRetract(), bSetMarkOffset(), bGetChOffset(),
 * Return Value:		Token
 ************************************************************/
Token tokenizer(void) {
	Token currentToken = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c;	/* input symbol */
	int state = 0;		/* initial state of the FSM */
	short lexStart;		/* start offset of a lexeme in the input char buffer (array) */
	short lexEnd;		/* end offset of a lexeme in the input char buffer (array)*/

	int lexLength;		/* token length */
	int i;				/* counter */
	unsigned char newc;	/* new char */

	while (1) { /* endless loop broken by token returns it will generate a warning */
		c = bGetCh(sourceBuffer);
		/* ------------------------------------------------------------------------
			Part 1: Implementation of token driven scanner.
			Every token is possessed by its own dedicated code
			-----------------------------------------------------------------------
		*/

		/*isspace() handles ' ', \n, \t, \v, \f, \r*/
		if (isspace(c)) {
			if (c == '\n') {
				line++;
			}
			continue;
		}
		switch (c) {
		case CHARSEOF0:
			currentToken.code = SEOF_T;
			currentToken.attribute.seofType = SEOF_0;
			return currentToken;
		case CHARSEOF255:
			currentToken.code = SEOF_T;
			currentToken.attribute.seofType = SEOF_EOF;
			return currentToken;
		case '%': /*Checks for the percentage character*/
			newc = bGetCh(sourceBuffer);
			if (newc == '%') { /*Checks for a second percentage character*/
				while (newc != CHARSEOF0 && newc != CHARSEOF255 && newc != '\n') {
					newc = bGetCh(sourceBuffer);
				}
				bRetract(sourceBuffer);/*Retracting char so it can be read again and be handled by isspace()*/
				continue;
			}
			else {
				bRetract(sourceBuffer); /*/Retracting char so it can be read again and be handled by isspace()*/
				currentToken.code = ERR_T; /*Gives an error if there is only a single percentage character*/
				currentToken.attribute.errLexeme[0] = '%';
				currentToken.attribute.errLexeme[1] = CHARSEOF0; /*Illegal symbol after %*/
				return currentToken;
			}
		case '{': /*Checks for the left brace character*/
			currentToken.code = LBR_T;
			return currentToken;
		case '}': /*Checks for the right brace character*/
			currentToken.code = RBR_T;
			return currentToken;
		case '=': /*Checks for the assignment operator*/
			newc = bGetCh(sourceBuffer);
			if (newc == '=') { /*Checks for the if EQUAL OPERATOR.*/
				/* Classify if is another == as EQUAL OPERATOR.*/
				currentToken.code = REL_OP_T;
				currentToken.attribute.relationalOperator = EQ; /*Assign token as if EQUAL OPERATOR*/
				return currentToken;
			}
			else {
				bRetract(sourceBuffer);
				currentToken.code = ASS_OP_T; /*Assign token as assignment operator*/
				return currentToken;
			}
			/*maybe retract buffer*/
		case '(': /*Checks for the left parenthesis character*/
			currentToken.code = LPR_T;
			return currentToken;
		case ')': /*Checks for the right parenthesis character*/
			currentToken.code = RPR_T;
			return currentToken;
		case ',': /*Checks for the coma character*/
			currentToken.code = COM_T;
			return currentToken;
		case ';': /*Checks for the semi-colon character*/
			currentToken.code = EOS_T;
			return currentToken;
		case '+': /*Checks for the addition operator*/
			newc = bGetCh(sourceBuffer);
			if (newc == '+') { /*Checks for a second addition operator*/
				currentToken.code = SCC_OP_T;
				return currentToken;
			}
			else {/*retract so it stays a single +*/
				bRetract(sourceBuffer);
				currentToken.code = ART_OP_T;
				currentToken.attribute.arithmeticOperator = ADD;
				return currentToken; /*Return the left brace token*/
			}
		case '-': /*Checks for minus operator*/
			currentToken.code = ART_OP_T;
			currentToken.attribute.arithmeticOperator = SUB;
			return currentToken;
		case '*': /*Checks for the multiplication operator*/
			currentToken.code = ART_OP_T;
			currentToken.attribute.arithmeticOperator = MUL;
			return currentToken;
		case '/': /*Checks for the devision operator*/
			currentToken.code = ART_OP_T;
			currentToken.attribute.arithmeticOperator = DIV;
			return currentToken;
		case '<': /*Checks for the smaller than operator*/
			currentToken.code = REL_OP_T;
			currentToken.attribute.relationalOperator = LT;
			return currentToken;
		case '>': /*Checks for the greater than operator*/
			currentToken.code = REL_OP_T;
			currentToken.attribute.relationalOperator = GT;
			return currentToken;
		case '!': /*Checks for the exclamation point character*/
			newc = bGetCh(sourceBuffer);
			if (newc == '=') { /*Checks for equal character afterwards*/
				currentToken.code = REL_OP_T;
				currentToken.attribute.relationalOperator = NE;
				return currentToken;
			}
			bRetract(sourceBuffer);
			currentToken.code = ERR_T; /*Gives an error if there is only an exlamation point*/
			currentToken.attribute.errLexeme[0] = '!';
			currentToken.attribute.errLexeme[1] = '\0';
			return currentToken;

		case '.': /*Checks for the period character*/
			bSetMarkOffset(sourceBuffer, bGetChOffset(sourceBuffer));
			newc = bGetCh(sourceBuffer); /*/ A or N or O */

			if (newc == 'A' && bGetCh(sourceBuffer) == 'N' && bGetCh(sourceBuffer) == 'D' && bGetCh(sourceBuffer) == '.') { /*Checks if the next characters are AND.*/
				currentToken.code = LOG_OP_T;
				currentToken.attribute.logicalOperator = AND; /*Set logicalOperator to AND*/
				return currentToken;
			}
			else if (newc == 'N' && bGetCh(sourceBuffer) == 'O' && bGetCh(sourceBuffer) == 'T' && bGetCh(sourceBuffer) == '.') { /*Checks if the next characters are NOT.*/
				currentToken.code = LOG_OP_T;
				currentToken.attribute.logicalOperator = NOT; /*Set logicalOperator to NOT*/
				return currentToken;
			}
			else if (newc == 'O' && bGetCh(sourceBuffer) == 'R' && bGetCh(sourceBuffer) == '.') { /*Checks if the next characters are OR.*/
				currentToken.code = LOG_OP_T;
				currentToken.attribute.logicalOperator = OR; /*Set logicalOperator to OR*/
				return currentToken;
			}
			else {
				bRestore(sourceBuffer);
				currentToken.code = ERR_T; /*If there is only a period or the next characters are not the ones above give an error*/
				currentToken.attribute.errLexeme[0] = '.';
				currentToken.attribute.errLexeme[1] = CHARSEOF0;
				return currentToken;
			}

			/* ------------------------------------------------------------------------
				Part 2: Implementation of Finite State Machine (DFA)
						   or Transition Table driven Scanner
						   Note: Part 2 must follow Part 1 to catch the illegal symbols
				-----------------------------------------------------------------------
			*/
			/* Transition driven scanner implementation inside default */
		default: /* general case*/
			state = nextState(state, c);
			lexStart = bGetChOffset(sourceBuffer) - 1;
			bSetMarkOffset(sourceBuffer, lexStart);
			/* keep getting char until it reaches an accept state*/
			while (stateType[state] == NOAS) {
				newc = bGetCh(sourceBuffer);
				state = nextState(state, newc);

			}

			if (stateType[state] == ASWR) {
				bRetract(sourceBuffer);
			}
			lexEnd = bGetChOffset(sourceBuffer);
			lexLength = lexEnd - lexStart;
			lexemeBuffer = bCreate((short)lexLength, state, 'f');
			bRestore(sourceBuffer);


			for (i = 0; i < lexLength; i++) {
				newc = bGetCh(sourceBuffer);
				bAddCh(lexemeBuffer, newc);
			}
			bFinish(lexemeBuffer, CHARSEOF0);
			currentToken = (*finalStateTable[state])(bGetContent(lexemeBuffer, 0));
			bFree(lexemeBuffer);
			return currentToken;
		}
	}

}


/* DO NOT MODIFY THE CODE / COMMENT OF THIS FUNCTION */
/*************************************************************
 * Get Next State
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	(*) assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:
	(*) Assertion failed: test, file filename, line linenum.
	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
 ************************************************************/

int nextState(int state, char c) {
	int col;
	int next;
	col = nextClass(c);
	next = transitionTable[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	assert(next != IS);
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	return next;
}

/*************************************************************
 * Purpose:				Identifies the class of a char and returns the appropriate column number from the transition table
 * Version:				1.0
 * Paramaters:			char
 * Functions Called:	isalpha(), isdigit()
 * Return Value:		int
 ************************************************************/
int nextClass(char c) {
	int val = -1;
	/*				[A-z](0),	[0-9](1),	.(2),	$(3),	'(4),	SEOF(5),	other(6) */
	if (isalpha(c)) {
		val = 0;
	}
	else if (isdigit(c)) {
		val = 1;
	}
	else if (c == '.') {
		val = 2;
	}
	else if (c == '$') {
		val = 3;
	}
	else if (c == '\'') {
		val = 4;
	}
	else if (c == CHARSEOF0 || c == CHARSEOF255) {
		val = 5;
	}
	else {
		val = 6;
	}
	return val;
}

/*************************************************************
 * Purpose:				Checks if the lexime is matches with the keyword from the language and if it does it returns the token. If it does not,
 * 						it checks to see if the lexime is a Arithmatic variable identifier(AVID) and then returns the token.
 * Version:				1.0
 * Paramaters:			char[]
 * Functions Called:	isKeyword(), strlen(), strcpy()
 * Return Value:		token
 ************************************************************/

Token funcAVID(char lexeme[]) {
	Token currentToken = { 0 };
	int i;
	if (isKeyword(lexeme) != RT_FAIL_1) {
		currentToken.code = KW_T;
		currentToken.attribute.keywordIndex = isKeyword(lexeme);
		return currentToken;
	}
	else {
		currentToken.code = AVID_T;
	}
	if (strlen(lexeme) > VID_LEN) {
		for (i = 0; i < VID_LEN; i++) {
			currentToken.attribute.vidLexeme[i] = lexeme[i];
		}
		currentToken.attribute.vidLexeme[VID_LEN] = CHARSEOF0;
	}
	else {
		strcpy(currentToken.attribute.vidLexeme, lexeme);
		currentToken.attribute.vidLexeme[strlen(lexeme)] = CHARSEOF0;
	}
	return currentToken;
}

/*************************************************************
 * Purpose:				Identifies if the lexeme is a string VID by checking if the SVID both starts and ends with a dollar sign($) and
 * 						sets the leximne to the correspounding attribute. It then returns the token.
 * 						If it does not respect the limit it returns an error otherwise it returns the token
 * Version:				1.0
 * Paramaters:			char[]
 * Functions Called:	strlen(), strcpy()
 * Return Value:		token
 ************************************************************/

Token funcSVID(char lexeme[]) {
	Token currentToken = { 0 };
	int i;
	currentToken.code = SVID_T;
	if (strlen(lexeme) > VID_LEN) {
		for (i = 0; i < VID_LEN; i++) {
			currentToken.attribute.vidLexeme[i] = lexeme[i];
		}
		currentToken.attribute.vidLexeme[VID_LEN - 1] = '$';
		currentToken.attribute.vidLexeme[VID_LEN] = CHARSEOF0;
	}
	else {
		strcpy(currentToken.attribute.vidLexeme, lexeme);
		currentToken.attribute.vidLexeme[strlen(lexeme)] = CHARSEOF0;
	}

	return currentToken;
}

/*************************************************************
 * Purpose:				Identifies if the lexeme is a Integer literal and if so, it checks if the lexeme respects the byte limit.
 * 						If it does not respect the limit it returns an error otherwise it sets the inValue to the lexeme and returns the token
 * Version:				1.0
 * Paramaters:			char[]
 * Functions Called:	atol(), funcErr()
 * Return Value:		token
 ************************************************************/

Token funcIL(char lexeme[]) {
	Token currentToken = { 0 };
	int sToInt = atol(lexeme); /* convert to int*/
	if (sToInt < SHRT_MIN || sToInt > SHRT_MAX) {
		currentToken = funcErr(lexeme);
	}
	else {

		currentToken.code = INL_T;
		currentToken.attribute.intValue = sToInt;
	}
	return currentToken;
}

/*************************************************************
 * Purpose:				Identifies if the lexeme is a floating point literal and if so, it checks if the lexeme respects the byte limit.
 * 						If it does not respect the limit it returns an error otherwise it returns the token
 * Version:				1.0
 * Paramaters:			char[]
 * Functions Called:	bGetAddChOffset(), strlen(), bAddCh()
 * Return Value:		token
 ************************************************************/

Token funcFPL(char lexeme[]) {
	Token currentToken = { 0 };
	float sToFPL = (float)atof(lexeme);
	if (((sToFPL >= 0 && strlen(lexeme) > 7) && (sToFPL < FLT_MIN || sToFPL > FLT_MAX)) || (sToFPL < 0)) {
		currentToken = funcErr(lexeme);
	}
	else {

		currentToken.code = FPL_T;
		currentToken.attribute.floatValue = (float)sToFPL;

	}
	return currentToken;
}

/*************************************************************
 * Purpose:				Identifies if the lexeme is a string literal if so it stores the lexeme into the stringLiteralTable and returns the current token
 * Version:				1.0
 * Paramaters:			char[]
 * Functions Called:	bGetAddChOffset(), strlen(), bAddCh()
 * Return Value:		token
 ************************************************************/
Token funcSL(char lexeme[]) {
	int i;
	Token currentToken = { 0 };
	currentToken.code = STR_T;
	/*Where first char will be added*/
	currentToken.attribute.contentString = bGetAddChOffset(stringLiteralTable);
	/*Copy lexeme to String Literal Table*/
	for (i = 0; i < (signed)strlen(lexeme); i++) {
		/*Ignore the opening and closing quotes*/
		if (!(lexeme[i] == '\'')) {
			bAddCh(stringLiteralTable, lexeme[i]);
		}
		if (lexeme[i] == '\n') {
			line++;
		}
	}
	/*  \0 to separate the lexemes*/
	bAddCh(stringLiteralTable, '\0');
	return currentToken;
}

/*************************************************************
 * Acceptance State Function Error
 *		Function responsible to deal with ERR token.
 * - This function uses the errLexeme, respecting the limit given
 *   by ERR_LEN. If necessary, use three dots (...) to use the
 *   limit defined. The error lexeme contains line terminators,
 *   so remember to increment line.
 ************************************************************/
 /*************************************************************
  * Purpose:				Function of which is responsible to deal with the ERR tokens.
  * Version:				1.0
  * Paramaters:			char[]
  * Functions Called:	strlen(), strcat()
  * Return Value:		token
  ************************************************************/
Token funcErr(char lexeme[]) {
	Token currentToken = { 0 };
	int i;
	/*if the lexeme is long than the defined limit, remove last 3 chars*/
	/*and append ... to indicate there is more*/

	if (strlen(lexeme) > ERR_LEN) {
		for (i = 0; i < (ERR_LEN - 3); i++) {
			if (lexeme[i] == '\n') {
				line++;
			}
			currentToken.attribute.errLexeme[i] = lexeme[i];
		}
		/*appending ...*/
		strcat(currentToken.attribute.errLexeme, "...");
	}
	else {
		for (i = 0; i < (signed)strlen(lexeme); i++) {
			if (lexeme[i] == '\n') {
				line++;
			}
			currentToken.attribute.errLexeme[i] = lexeme[i];
		}
	}

	currentToken.code = ERR_T;


	return currentToken;
}


/*************************************************************
 * Purpose:				Checks if the lexeme is a keyword
 * Version:				1.0
 * Paramaters:			char[]
 * Functions Called:	strlen(), strcmp()
 * Return Value:		int
 ************************************************************/
int isKeyword(char lexeme[]) {
	int i;
	if (!strlen(lexeme)) {
		return RT_FAIL_1;

	}
	for (i = 0; i < KWT_SIZE; i++) {
		if (strcmp(lexeme, keywordTable[i]) == 0) {
			return i;
		}
	}
	return RT_FAIL_1;
}