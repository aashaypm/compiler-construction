#ifndef _STACK_
#define _STACK_

#include "lexerDef.h"
#include "lexer.h"
#include "parserDef.h"

typedef struct attNode{
	tnt_tag t;
	grSymbol s;
} *AttNode;

typedef struct stackNode{
	AttNode grammar_node;
	struct stackNode *next;
} *StackNode;

typedef struct stack{
	StackNode head;
	int size;
} *Stack;

Stack newStack();
Stack push(AttNode grammar_node, Stack st);
Stack pop(Stack st);
AttNode top(Stack st);
int isEmpty(Stack st);

#endif