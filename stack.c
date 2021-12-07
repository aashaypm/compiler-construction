#include "stack.h"

Stack newStack(){
	Stack st = (Stack) malloc(sizeof(struct stack));
	st->head = NULL;
	st->size = 0;
	return st;
}

int isEmpty(Stack st){
	return st->head == NULL;
}

Stack push(AttNode grammar_node, Stack st){
	StackNode stNode = (StackNode) malloc(sizeof(struct stackNode));
	stNode->grammar_node = grammar_node;

	//insert at front
	stNode->next = st->head;
	st->head = stNode;
	st->size++;
	return st;
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

AttNode top(Stack st){
	if(st->head == NULL)
		return NULL;

	return st->head->grammar_node;
}