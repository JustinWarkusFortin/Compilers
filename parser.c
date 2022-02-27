/*************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Winter, 2021
*************************************************************
* File name: parser.c
* Compiler: MS Visual Studio 2019
* Author: Justin Fortin, Kalen Gladu-Lauridsen
* Course: CST 8152 – Compilers, Lab Section: [014]
* Assignment: A3.
* Date: April 18 2021
* Purpose: This file is the main table implementation for Parser (.c)
* Function list: (
					void startParser(),
					matchToken(),
					syncErrorHandler(),
					printError(),
					void printMessage(),
					 program(),
					 optionalStatements(),
					 statements(),
					 statementsPrime(),
					 statement(),
					 assignmentStatement(),
					 assignmentExpression(),
					 selectionStatememnt(),
					 iterationStatement(),
					 preCondition(),
					 inputStatement(),
					 variableList(),
					 variableListPrime(),
					 variableIdentifier(),
					 outputStatement(),
					 outputStatementPrime(),
					 optVariableList(),
					 outputList(),
					 arithmeticExpression(),
					 unaryArithmeticExpression(),
					 additiveArithmaticExpression(),
					 additiveArithmaticExpressionPrime(),
					 multiplicativeArithmaticExpression(),
					 multiplicativeArithmaticExpressionPrime(),
					 primaryArithmaticExpression(),
					 stringExpression(),
					 stringExpressionPrime(),
					 primaryStringExpression(),
					 conditionalExpression(),
					 logicalOrExpression(),
					 logicalOrExpressionPrime(),
					 logicalAndExpression(),
					 logicalAndExpressionPrime(),
					 logicalNotExpression(),
					 relationalExpression(),
					 relationalAExpression(),
					 relationalAExpressionPrime(),
					 primaryARelationalExpression(),
					 relationalSExpression(),
					 relationalSExpressionPrime(),
					 primarySRelationalExpression()
).
*************************************************************/
/* TODO_201: Adjust the function header */

#include "parser.h"

/*************************************************************
 * Process Parser
 ************************************************************/
void startParser(void) {
	lookahead = tokenizer();
	program();
	matchToken(SEOF_T, NO_ATTR);
	printf("%s\n", "PLATY: Source file parsed");
}

/*************************************************************
 * Match Token
 ************************************************************/
 /* TODO_202: Continue the development */
void matchToken(int tokenCode, int tokenAttribute) {
	int matchFlag = 1; /* dont know how to implement*/

	if (lookahead.code != tokenCode) {
		matchFlag = 0;
	}
	switch (lookahead.code) {
	case KW_T: case LOG_OP_T: case ART_OP_T: case REL_OP_T:
		if (lookahead.attribute.codeType != tokenAttribute) {
			matchFlag = 0;
		}
		break;
	}
	if (matchFlag && lookahead.code == SEOF_T)
		return;
	if (matchFlag) {
		lookahead = tokenizer();
		if (lookahead.code == ERR_T) {
			printError();
			lookahead = tokenizer();
			syntaxErrorNumber++;
		}
	}
	else
		syncErrorHandler(tokenCode);
}

/*************************************************************
 * Syncronize Error Handler
 ************************************************************/
 /* TODO_203: Continue the development */
void syncErrorHandler(int syncTokenCode) {
	printError();
	syntaxErrorNumber++;
	while (lookahead.code != syncTokenCode) {
		if (lookahead.code == SEOF_T) {
			exit(syntaxErrorNumber);
		}
		lookahead = tokenizer();
	}
	if (lookahead.code != SEOF_T) {
		lookahead = tokenizer();
	}
}

/*************************************************************
 * Print Error
 ************************************************************/
 /* TODO_204: Continue the development */
void printError() {
	Token t = lookahead;
	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case ERR_T: /*Error Token */
		printf("%s\n", t.attribute.errLexeme);
		break;
	case SEOF_T:/*Source End of File 0*/
		printf("NA\n");
		break;
	case AVID_T: case SVID_T:/*Arithmatic and String Variable Identifiers 1 ,2*/
		printf("%s\n", t.attribute.vidLexeme);
		break;
	case FPL_T: /*Floating point literal 3*/
		printf("%5.1f\n", t.attribute.floatValue);
		break;
	case INL_T:/* Integer literal token 4*/
		printf("%d\n", t.attribute.intValue);
		break;
	case STR_T:/* String literal token 5*/
		printf("%s\n", bGetContent(stringLiteralTable, t.attribute.contentString));
		break;
	case SCC_OP_T: /* String concatenation operator token: ++ 6*/
		printf("NA\n");
		break;
	case ASS_OP_T: /* Assignment operator token 7*/
		printf("NA\n");
		break;
	case ART_OP_T:  /* Arithmetic operator token 8*/
		printf("%d\n", t.attribute.arithmeticOperator);
		break;
	case REL_OP_T: /* Relational operator token 9*/
		printf("%d\n", t.attribute.relationalOperator);
		break;
	case LOG_OP_T:/* Logical operator token 10*/
		printf("%d\n", t.attribute.logicalOperator);
		break;
	case LPR_T: /* Left parenthesis token 11*/
		printf("NA\n");
		break;
	case RPR_T: /* Right parenthesis token 12*/
		printf("NA\n");
		break;
	case LBR_T:/* Left brace token 13*/
		printf("NA\n");
		break;
	case RBR_T:  /* Right brace token 15*/
		printf("NA\n");
		break;
	case KW_T:  /* Keyword token 16*/
		printf("%s\n", keywordTable[t.attribute.keywordIndex]);
		break;
	case COM_T: /* Comma token 17*/
		printf("NA\n");
		break;
	case EOS_T:/* End of statement (semicolon) 18*/
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}
}

/* TODO_205: Continue the development (all non-terminal functions) */

/*************************************************************
 * Program statement
 * <program> -> PLATYPUS { <opt_statements> }
 * FIRST(<program>)= {KW_T (MAIN)}.
 ************************************************************/
void program(void) {
	matchToken(KW_T, MAIN);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	printf("%s\n", "PLATY: Program parsed");
}
/*************************************************************
 * Optional statement

 * <opt_statements> ->  <statements> | ϵ
 * FIRST(<opt_statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE), ϵ }
 ************************************************************/
void optionalStatements(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:
		statements();
		break;
	case KW_T:
		if (lookahead.attribute.codeType == IF
			|| lookahead.attribute.codeType == WHILE
			|| lookahead.attribute.codeType == READ
			|| lookahead.attribute.codeType == WRITE) {
			statements();
		}
		break;
	}
	printf("%s\n", "PLATY: Optional statements parsed");
}
/*************************************************************
* Statements

* <statementsPrime> ->  <statement><statementsPrime> | ϵ
* FIRST(<statementsPrime>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE), ϵ }
 ************************************************************/
void statements(void) {
	statement();
	statementsPrime();
	printf("%s\n", "PLATY: Statements parsed");
}
/*************************************************************
 * Statements Prime

 * <statementsPrime> ->  <statement><statementsPrime> | ϵ
 * FIRST(<statementsPrime>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE), ϵ }
 ************************************************************/
void statementsPrime(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:
		statement();
		statementsPrime();
		break;
	case KW_T:
		if (lookahead.attribute.codeType == IF ||
			lookahead.attribute.codeType == WHILE ||
			lookahead.attribute.codeType == READ ||
			lookahead.attribute.codeType == WRITE) {
			statement();
			statementsPrime();
		}
		break;
	}
}
/*************************************************************
 * Statement

 * <statement> ->  <assignment statement> | <selection statement> | <iteration statement> | <input statement> | <output statement>
 * FIRST(<statement>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE) }
 ************************************************************/
void statement(void) {
switch (lookahead.code) {
	case AVID_T: case SVID_T:
		assignmentStatement();
		break;
	case KW_T:
		switch (lookahead.attribute.codeType) {
		case IF: 
			selectionStatement(); 
			break;
		case WHILE: 
			iterationStatement(); 
			break;
		case READ: 
			inputStatement(); 
			break;
		case WRITE: 
			outputStatement(); 
			break;
		}
		break;
	default:
		printError();
		
	}
	printf("%s\n", "PLATY: Statement parsed");
}

/*************************************************************
 * Assignment Statement

 * <assignment statement> -> <assignment expression>;
 * FIRST(<assignment statement>) = { AVID_T, SVID_T }
 ************************************************************/
void assignmentStatement(void) {
	assignmentExpression();
	matchToken(EOS_T, NO_ATTR);
	printf("%s\n", "PLATY: Assignment statement parsed");
}
/*************************************************************
 * Assignment Expression

 * <assignment expression> -> AVID = <arithmetic expression> | SVID = <string expression>
 * FIRST(<assignment expression>) = { AVID_T, SVID_T }
 ************************************************************/
void assignmentExpression(void) {
	switch (lookahead.code) {
	case AVID_T:
		matchToken(AVID_T, NO_ATTR);
		matchToken(ASS_OP_T, NO_ATTR);
		arithmeticExpression();
		printf("%s\n", "PLATY: Assignment expression parsed");
		break;
	case SVID_T:
		matchToken(SVID_T, NO_ATTR);
		matchToken(ASS_OP_T, NO_ATTR);
		stringExpression();
		printf("%s\n", "PLATY: Assignment expression parsed");
		break;
	default:
		printError();
	}
}
/*************************************************************
 * Selection Statement

 * <selection statement> -> IF <pre-condition> (<conditional expression>) THEN { <opt_statements> } ELSE { <opt_statements> };
 * FIRST(<selection statement>) = { KW_T(IF) }
 ************************************************************/
void selectionStatement(void) {
	/*Matching If*/
	matchToken(KW_T, IF);
	preCondition();
	matchToken(LPR_T, NO_ATTR);
	conditionalExpression();
	matchToken(RPR_T, NO_ATTR);

	/*Matching Then*/
	matchToken(KW_T, THEN);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);

	/*Matching Else*/
	matchToken(KW_T, ELSE);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);

	printf("%s\n", "PLATY: Selection statement parsed");
}
/*************************************************************
 * Iteration Statement

 * <iteration statement> -> WHILE <pre-condition> (<conditional expression>) DO { <statements>};
 * FIRST(<iteration statement>) = { KW_T(WHILE) }
 ************************************************************/
void iterationStatement(void) {
	/*Matching keyword While*/
	matchToken(KW_T, WHILE);
	preCondition();
	matchToken(LPR_T, NO_ATTR);
	conditionalExpression();
	matchToken(RPR_T, NO_ATTR);
	/*Matching keyword Do*/
	matchToken(KW_T, DO);
	matchToken(LBR_T, NO_ATTR);
	statements();
	matchToken(RBR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);

	printf("%s\n", "PLATY: Iteration statement parsed");

}
/*************************************************************
 * Pre Condition

 * <pre condition> -> TRUE | FALSE
 * FIRST(<pre condition>) = { KW_T(TRUE), KW_T(FALSE) }
 ************************************************************/
 //maybe change
void preCondition(void) {
	if (lookahead.code == KW_T) {
		switch (lookahead.attribute.codeType) {
		case TRUE: case FALSE:
			matchToken(KW_T, lookahead.attribute.codeType);
			printf("%s\n", "PLATY: Pre-condition parsed");
			break;
		default:
			printError();
		}
	} else
		printError();
}
/*************************************************************
 * Input Statement

 * <input statement> -> READ (<variable list>);
 * FIRST(<input statement>) = { KW_T(READ) }
 ************************************************************/
void inputStatement(void) {
	matchToken(KW_T, READ);
	matchToken(LPR_T, NO_ATTR);
	variableList();
	matchToken(RPR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	printf("%s\n", "PLATY: Input statement parsed");
}
/*************************************************************
 * Variable List

 * <variable list> -> <variable identifier><variable list Prime>
 * FIRST(<variable list>) = { FIRST(<variable identifier>} 
		          = { AVID_T, SVID_T }}

 ************************************************************/
void variableList(void) {
	variableIdentifier();
	variableListPrime();
	printf("%s\n", "PLATY: Variable list parsed");
}
/*************************************************************
 * Variable List Prime

 * <variable list Prime> -> ,<variable identifier><variable list Prime> | ϵ
 * <variable list Prime> -> ,<variable identifier><variable list Prime> | ϵ
 ************************************************************/
void variableListPrime(void) {
	if (lookahead.code == COM_T) {
		matchToken(COM_T, NO_ATTR);
		variableIdentifier();
		variableListPrime();
	}
}
/*************************************************************
 * Variable Identifier

 * <variable identifier>  AVID_T | SVID_T 
 * FIRST(<variable identifier>) = { AVID_T, SVID_T }
 ************************************************************/
void variableIdentifier(void) {
	switch (lookahead.code) {
	case AVID_T:
		matchToken(AVID_T, NO_ATTR);
		break;
	case SVID_T:
		matchToken(SVID_T, NO_ATTR);
		break;
	default:
		printError();
	}
	printf("%s\n", "PLATY: Variable identifier parsed");
}
/*************************************************************
 * Output Statement

 * <output statement>  WRITE (<output statementPrime>);
 * FIRST(<output statement>) = { KW_T(WRITE) }
 ************************************************************/
void outputStatement(void) {
	matchToken(KW_T, WRITE);
	matchToken(LPR_T, NO_ATTR);
	outputStatementPrime();
	matchToken(RPR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	printf("%s\n", "PLATY: Output statement parsed");
}
/*************************************************************
 * Output Statement Prime

 * <output statementPrime> -> <opt_variable list> | STR_T
 * FIRST(<output statementPrime>) = { FIRST(<opt_variable list>) 
		                = { AVID_T, SVID_T, ϵ , STR_T}}

 ************************************************************/
void outputStatementPrime(void) {
	switch (lookahead.code) {
	case STR_T:
		matchToken(STR_T, NO_ATTR);
		printf("%s\n", "PLATY: Output variable list parsed");
		break;
	default:
		optVariableList();
	}
}
/*************************************************************
 * Opt Variable List

 * <opt_variable list> -> <variable list> | ϵ 
 * FIRST(<opt_variable list>) = { FIRST(<variable list>) 
		                 = { AVID_T, SVID_T, ϵ }}

 ************************************************************/
void optVariableList(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:
		variableList();
		break;
	}
	printf("%s\n", "PLATY: Output variable list parsed");
}
/*************************************************************
 * Arithmatic Expression

 * <arithmetic expression>   <unary arithmetic expression> | <additive arithmetic expression>	
 * FIRST(<arithmetic expression>) = { FIRST(<unary arithmetic expression>) 
                                                    = { ART_OP_T(SUB), ART_OP_T(ADD) }, 
                                                         FIRST(<additive arithmetic expression>) 
                                                    = { AVID_T, FPL_T, INL_T, LPR_T }} 

 ************************************************************/
void arithmeticExpression(void) {
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T: case LPR_T:
		additiveArithmaticExpression();
		printf("%s\n", "PLATY: Arithmetic expression parsed");
		break;
	case ART_OP_T:
		switch (lookahead.attribute.arithmeticOperator) {
		case ADD: case SUB:
			unaryArithmeticExpression();
			printf("%s\n", "PLATY: Arithmetic expression parsed");
			break;
		default:
			printError();
		}
	}
}


/*************************************************************
 * Unary Arithmatic Expression

 * <unary arithmetic expression> -> - <primary arithmetic expression> 
 				| + <primary arithmetic expression>
 * FIRST(<unary arithmetic expression>) = { ART_OP_T(SUB), ART_OP_T(ADD) }
 ************************************************************/
void unaryArithmeticExpression(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arithmeticOperator) {
		case ADD:
			matchToken(ART_OP_T, ADD);
			primaryArithmaticExpression();
			break;
		case SUB:
			matchToken(ART_OP_T, SUB);
			primaryArithmaticExpression();
			break;
		default:
			printError();
		}
		break;
	default:
		printError();
	}
}
/*************************************************************
 * Additive Arithmatic Expression

 * <additive arithmetic expression> -> <multiplicative arithmetic expression><additive arithmetic expression Prime>
 * FIRST(<additive arithmetic expression>) = { FIRST(<multiplicative arithmetic expression>) 
                                                                 = { AVID_T, FPL_T, INL_T, LPR_T }}

 ************************************************************/
void additiveArithmaticExpression(void) {
	multiplicativeArithmaticExpression();
	additiveArithmaticExpressionPrime();
	printf("%s\n", "PLATY: Additive arithmetic expression parsed");

}
/*************************************************************
 * Additive Arithmatic Expression Prime

 * <additive arithmetic expression Prime> -> 
+<multiplicative arithmetic expression><additive arithmetic expression Prime> | 
-<multiplicative arithmetic expression><additive arithmetic expression Prime> | ϵ
 * FIRST(<additive arithmetic expression Prime>) = { ART_OP_T(ADD), ART_OP_T(SUB), ϵ }
 ************************************************************/
void additiveArithmaticExpressionPrime(void) {

//maybe change back to hardcode -
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arithmeticOperator) {
		case ADD:
			matchToken(ART_OP_T, lookahead.attribute.arithmeticOperator);
			multiplicativeArithmaticExpression();
			additiveArithmaticExpressionPrime();
			break;
		case SUB:
			matchToken(ART_OP_T, lookahead.attribute.arithmeticOperator);
			multiplicativeArithmaticExpression();
			additiveArithmaticExpressionPrime();
			break;

		}
	}
}
/*************************************************************
 * Multiplicative Arithmatic Expression

 * <multiplicative arithmetic expression> -> <primary arithmetic expression><multiplicative arithmetic expression Prime>
 * FIRST(<multiplicative arithmetic expression>) = { FIRST(<primary arithmetic expression>) 
                                                 = { AVID_T, FPL_T, INL_T, LPR_T }}
 ************************************************************/
void multiplicativeArithmaticExpression(void) {
	primaryArithmaticExpression();
	multiplicativeArithmaticExpressionPrime();
	printf("%s\n", "PLATY: Multiplicative arithmetic expression parsed");
}
/*************************************************************
 * Multiplicative Arithmatic Expression Prime

 * <multiplicative arithmetic expression Prime> -> 
  *<primary arithmetic expression><multiplicative arithmetic expression Prime>  
| /<primary arithmetic expression><multiplicative arithmetic expression Prime> 
| ϵ

 * FIRST(<multiplicative arithmetic expression Prime>) = { ART_OP_T(DIV), ART_OP_T(DIV), ϵ }
 ************************************************************/
void multiplicativeArithmaticExpressionPrime(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arithmeticOperator) {
		case MUL:
			matchToken(ART_OP_T, MUL);
			primaryArithmaticExpression();
			multiplicativeArithmaticExpressionPrime();
			break;
		case DIV:
			matchToken(ART_OP_T, DIV);
			primaryArithmaticExpression();
			multiplicativeArithmaticExpressionPrime();
			break;
		}
	}
}
/*************************************************************
 * Primary Arithmatic Expression

 * <primary arithmetic expression> -> AVID_T | FPL_T | INL_T
 					| (<arithmetic expression>)

 * FIRST(<primary arithmetic expression>) = { AVID_T, FPL_T, INL_T, LPR_T }
 ************************************************************/
void primaryArithmaticExpression(void) {
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T:
		matchToken(lookahead.code, NO_ATTR);
		printf("%s\n", "PLATY: Primary arithmetic expression parsed");
		break;
	case LPR_T:
		matchToken(LPR_T, NO_ATTR);
		arithmeticExpression();
		matchToken(RPR_T, NO_ATTR);
		printf("%s\n", "PLATY: Primary arithmetic expression parsed");
		break;
	default:
		printError();
	}
}
/*************************************************************
 * String Expression

 * <string expression> -> <primary string expression><string expression Prime>
*  FIRST(<string expression>) = { FIRST(<primary string expression>) }
			      = { SVID_T, STR_T }

 ************************************************************/
void stringExpression(void) {
	primaryStringExpression();
	stringExpressionPrime();
	printf("%s\n", "PLATY: String expression parsed");
}
/*************************************************************
 * String Expression Prime

 * <string expression Prime> -> ++<primary string expression><string expression Prime> | ϵ
 * FIRST(<string expression Prime>) = { SCC_OP_T, ϵ }
 ************************************************************/
void stringExpressionPrime(void) {
	switch (lookahead.code) {
	case SCC_OP_T:
		matchToken(SCC_OP_T, NO_ATTR);
		primaryStringExpression();
		stringExpressionPrime();
		break;
	}
}
/*************************************************************
 * Primary String Expression

 * <primary string expression>    SVID_T | STR_T
 * FIRST(<primary string expression>) = { SVID_T, STR_T }
 ************************************************************/
void primaryStringExpression(void) {
	switch (lookahead.code) {
	case SVID_T: case STR_T:
		matchToken(lookahead.code, NO_ATTR);
		printf("%s\n", "PLATY: Primary string expression parsed");
		break;
	default:
		printError();
	}
}
/*************************************************************
 * Conditional Expression

 * <conditional expression>   <logical OR expression>
 * FIRST(<conditional expression>) = { FIRST(<logical OR expression>) 
                                                     = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }}

 ************************************************************/
void conditionalExpression(void) {
	logicalOrExpression();
	printf("%s\n", "PLATY: Conditional expression parsed");
}
/*************************************************************
 * Logical Or Expression

 * <logical OR expression> -> <logical AND expression><logical OR expression Prime>
 * FIRST(<logical OR expression>) = { FIRST(<logical AND expression>) 
                                        = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }}}

 ************************************************************/
void logicalOrExpression(void) {
	logicalAndExpression();
	logicalOrExpressionPrime();
	printf("%s\n", "PLATY: Logical Or Expression parsed");
}
/*************************************************************
 * Logical Or Expression Prime

 * <logical OR expression Prime> -> .OR.<logical AND expression><logical OR expression Prime> | ϵ
 * FIRST(<logical OR expression Prime>) = {LOG_OP_T(OR), ϵ }
 ************************************************************/
void logicalOrExpressionPrime(void) {
	switch (lookahead.code) {
	case LOG_OP_T:
		switch (lookahead.attribute.logicalOperator) {
		case OR:
			matchToken(LOG_OP_T, OR);
			logicalAndExpression();
			logicalOrExpressionPrime();
			printf("%s\n", "PLATY: Logical OR expression parsed");
			break;
		}
		break;
	}
}
/*************************************************************
 * Logical And Expression

 * <logical AND expression> -> <logical NOT expression><logical AND expression Prime>
 * FIRST(<logical AND expression>) = { FIRST(<relational expression>) 
				   = { AVID_T, FPL_T, INL_T, SVID_T, STR_T } }

 ************************************************************/
void logicalAndExpression(void) {
	logicalNotExpression();
	logicalAndExpressionPrime();
	printf("%s\n", "PLATY: Logical And Expression parsed");
}
/*************************************************************
 * Logical And Expression Prime

 * <logical AND expression Prime> -> .AND.<logical NOT expression><logical AND expression Prime> | ϵ
 * FIRST(<logical AND expression Prime>) = { LOG_OP_T(AND), ϵ }
 ************************************************************/
void logicalAndExpressionPrime(void) {
	switch (lookahead.code) {
	case LOG_OP_T:
		switch (lookahead.attribute.logicalOperator) {
		case AND:
			matchToken(LOG_OP_T, AND);
			logicalNotExpression();
			logicalAndExpressionPrime();
			printf("%s\n", "PLATY: Logical AND expression parsed");
			break;
		}
		break;
	}
}
/*************************************************************
 * Logical Not Expression

 * <logical NOT expression> ->  .NOT. <relational expression> 		
    					| <relational expression>

 * FIRST(<logical NOT expression>) = { LOG_OP_T(NOT), 
                                                         FIRST(<relational expression>) 
				   = { AVID_T, FPL_T, INL_T, SVID_T, STR_T } }

 ************************************************************/
void logicalNotExpression(void) {
	switch (lookahead.code) {
	case LOG_OP_T:
		matchToken(LOG_OP_T, NOT);
		relationalExpression();
		break;
	case AVID_T: case SVID_T: case FPL_T: case INL_T: case STR_T:
		relationalExpression();
		break;
	default:
		printError();
		printf("%s\n", "PLATY: Relational expression parsed");
	}
	printf("%s\n", "PLATY: Logical Not Expression parsed");

}
/*************************************************************
 * Relational Expression

 *  	<relational expression> -> <relational a_expression>  | <relational s_expression>
 * FIRST(<relational expression>) = { FIRST(<relational a_expression>) 
                                                   = { AVID_T, FPL_T, INL_T } }, 
                                                      FIRST(<relational s_expression>) 
                                                   = { SVID_T, STR_T }}

 ************************************************************/
void relationalExpression(void) {
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T:
		relationalAExpression();
		printf("%s\n", "PLATY: Relational expression parsed");
		break;
	case SVID_T: case STR_T:
		relationalSExpression();
	//	primarySRelationalExpression();
		printf("%s\n", "PLATY: Relational expression parsed");
		break;
	default:
		printError();
	}
}
/*************************************************************
 * Relational A Expression

 * <relational a_expression> -> <primary a_relation_expression> <relational a_expression Prime>  <primary a_relation_expression>
 * FIRST(<relational a_expression>) = { FIRST(<primary a_relation expression>) }
				   = { AVID_T, FPL_T, INL_T }

 ************************************************************/
void relationalAExpression(void) {
	primaryARelationalExpression();
	relationalAExpressionPrime();
	primaryARelationalExpression();
	printf("%s\n", "PLATY: Relational arithmetic operator parsed");
	printf("%s\n", "PLATY: Relational arithmetic expression parsed");
}
/*************************************************************
 * Relational A Expression Prime

 * <relational a_expression Prime> -> == | != | > | <
 * FIRST(<relational a_expression Prime>) = { REL_OP_T(EQ), REL_OP_T(NE), REL_OP_T(GT), REL_OP_T(LT) }
 ************************************************************/
void relationalAExpressionPrime(void) {
	if (lookahead.code == REL_OP_T) {
		matchToken(REL_OP_T, lookahead.attribute.relationalOperator);
	}
	else {
		printError();
	}
}
/*************************************************************
 * Primary A Relational Expression

 * <primary a_relational expression>   AVID_T  | FPL_T  | INL_T
*  FIRST(<primary a_relational expression>) = { AVID_T, FPL_T, INL_T } 
************************************************************/
void primaryARelationalExpression(void) {
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T:
		matchToken(lookahead.code, NO_ATTR);
		printf("%s\n", "PLATY: Primary relational arithmetic expression parsed");
		break;
	}
}
/*************************************************************
 * Relational S Expression

 * <relational s_expression> -> <primary s_relational expression><relational s_expression Prime> <primary s_relational expression>
 * FIRST(<relational s_expression>) = { FIRST(<primary s_relational expression>) }
				   = { SVID_T, STR_T }

 ************************************************************/
void relationalSExpression(void) {
	primarySRelationalExpression();
	relationalSExpressionPrime();
	primarySRelationalExpression();
	printf("%s\n", "PLATY: Relational string operator parsed");
	printf("%s\n", "PLATY: Relational string expression parsed");
}
/*************************************************************
 * Relational S Expression Prime

 * <relational s_expression Prime> -> == | != | > | <
 * FIRST(<relational s_expression Prime>) = { REL_OP_T(EQ), REL_OP_T(NE), REL_OP_T(GT), REL_OP_T(LT) }
 ************************************************************/
void relationalSExpressionPrime(void) {
	if (lookahead.code == REL_OP_T) {
		matchToken(REL_OP_T, lookahead.attribute.relationalOperator);
	}
	else {
		printError();
	}
}
/*************************************************************
 * Primary S Relational Expression

 *<primary s_relational expression>  <primary string expression>
 * FIRST(<primary s_relational expression>) = { FIRST(<primary string expression>) }
					   = { SVID_T, STR_T }

 ************************************************************/
void primarySRelationalExpression(void) {
	primaryStringExpression();
	printf("%s\n", "PLATY: Primary relational string expression parsed");
}

