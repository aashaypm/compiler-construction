/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#ifndef _PARSEDEF_
#define _PARSEDEF_

#include "lexerDef.h"

#define TOTAL_NUM_NT 51
#define RULE_COUNT 92

typedef enum{
	program,
	mainFunction,
	otherFunctions,
	function,
	input_par,
	output_par,
	parameter_list,
	dataType,
	remaining_list,
	primitiveDatatype,
	constructedDatatype,
	stmts,
	typeDefinitions,
	typeDefinition,
	fieldDefinitions,
	fieldDefinition,
	moreFields,
	declarations,
	declaration,
	global_or_not,
	otherStmts,
	stmt,
	assignmentStmt,
	singleOrRecId,
	new24,
	funCallStmt,
	outputParameters,
	inputParameters,
	iterativeStmt,
	conditionalStmt,
	elsePart,
	ioStmt,
	allVar,
	newVar,
	arithmeticExpression,
	expPrime,
	term,
	termPrime,
	factor,
	highPrecedenceOperators,
	lowPrecedenceOperators,
	all,
	temp,
	booleanExpression,
	var,
	logicalOp,
	relationalOp,
	returnStmt,
	optionalReturn,
	idList,
	more_ids
}TK_NTTYPES;

typedef union
{
	TK_TYPES term_type;
	TK_NTTYPES nonterm_type;
}grSymbol;

typedef enum{TERM, NON_TERM} tnt_tag;

typedef struct grNode{
	tnt_tag t;
	grSymbol s;
	struct grNode* next;
	struct grNode* prev;
}* GrNode;

typedef struct grHead{
	TK_NTTYPES nonterm_head;
	GrNode first;
	GrNode last;
}* GrHead;

typedef GrHead Grammar;

#endif