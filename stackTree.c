/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#include "stackTree.h"
int parseTreeMem = 0;
int parseTreeNodes = 0;

Stack newStack(){
	Stack st = (Stack) malloc(sizeof(struct stack));
	st->head = NULL;
	st->size = 0;
	return st;
}

int isEmpty(Stack st){
	return st->head == NULL;
}

Stack push(StackNode stNode, Stack st){
	//insert at front
	stNode->next = st->head;
	st->head = stNode;
	st->size++;
	return st;
}
StackNode createNode(AttNode grammar_node)
{
	StackNode stNode = (StackNode) malloc(sizeof(struct stackNode));
	stNode->grammar_node = grammar_node;
	stNode->next=NULL;
	stNode->parent=NULL;
	stNode->right=NULL;
	stNode->child=NULL;
	stNode->inh=NULL;
	stNode->syn=NULL;
	stNode->addr=NULL;
	stNode->rule_id = -2;
	parseTreeMem += sizeof(stNode);
	parseTreeNodes += 1;
	return stNode;
}

Stack pop(Stack st){
	if(st->head == NULL){
		st->size = 0;
		return st;
	}
	StackNode temp = st->head;
	st->head = temp->next;
	st->size--;
	return st;
}

StackNode top(Stack st){
	if(st->head == NULL)
		return NULL;

	return st->head;
}
