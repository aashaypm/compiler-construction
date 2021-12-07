/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#ifndef _STACK_
#define _STACK_

#include "lexerDef.h"
#include "lexer.h"
#include "parserDef.h"

#define RECORD_REGS 16

//these two structures are used in ast
typedef enum {intType, realType, recordType, errType} varTypes;

typedef struct typeList{
	struct typeList* next;
	varTypes vtype;
	char fieldName[MAX_LEXEME_SIZE + 1];
}* TypeList;


typedef struct astNode{
    struct astNode *parent;
    struct astNode *right;
    struct astNode *child;
	//struct astNode *last; //last node of linked list for a construct
    //int offset;
    grSymbol s;
   	char value[MAX_FLOAT_LEN+1];
    tnt_tag t;
	TypeList tlist;
    int globalFlg; //global flag, 0 init 1 means global
	int line_number;
	char recName[MAX_LEXEME_SIZE+1];  //store record name for declarations of record type

	//for codegeneration
	int intreg;   //initialized to -1
	int regLocations[RECORD_REGS];
} *AstNode;

typedef struct attNode{
	tnt_tag t;
	grSymbol s;
} *AttNode;

typedef struct stackNode{
	AttNode grammar_node;
	struct stackNode* next;
	struct stackNode* parent;
	struct stackNode* right;
	struct stackNode* child;
	char value[MAX_FLOAT_LEN+1];
	int line;

	//for ast construction
	AstNode inh, syn, addr;
	int rule_id; // default value is -2, follows 0 based indexing
} *StackNode;

typedef struct stack{
	StackNode head;
	int size;
} *Stack;

typedef StackNode TreeNode;

TreeNode createTree(TreeNode root);

Stack newStack();
Stack push(StackNode stNode, Stack st);
Stack pop(Stack st);
StackNode top(Stack st);
int isEmpty(Stack st);
StackNode createNode(AttNode grammar_node);

//ast helper function
AstNode createAstNode(TreeNode t);

//creat ast
void createAst(TreeNode root);

//print ast
void printAst(AstNode root);

TypeList createTypeList(varTypes vtype);
#endif
