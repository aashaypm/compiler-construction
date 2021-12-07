/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#include "lexerDef.h"

void addKeyword(HashTable ht, char* key, TK_TYPES type_of_token);
int hash(char* key);
TK_TYPES findKeyword(HashTable ht, char* key);

HashTable ht;

HashTable initializeHashTable()
{
	ht = (HashTable)malloc(sizeof(htentry)*HT_SIZE);
	int i;
	for(i = 0; i < HT_SIZE; i++)
	{
		ht[i].isvalid = 0;
		ht[i].next = NULL;
	}

	addKeyword(ht,"with",TK_WITH);
	addKeyword(ht,"parameters",TK_PARAMETERS);
	addKeyword(ht,"end",TK_END);
	addKeyword(ht,"while",TK_WHILE);	
	addKeyword(ht,"type",TK_TYPE);
	addKeyword(ht,"_main",TK_MAIN);
	addKeyword(ht,"global",TK_GLOBAL);
	addKeyword(ht,"parameter",TK_PARAMETER);
	addKeyword(ht,"list",TK_LIST);
	addKeyword(ht,"input",TK_INPUT);
	addKeyword(ht,"output",TK_OUTPUT);
	addKeyword(ht,"int",TK_INT);
	addKeyword(ht,"real",TK_REAL);
	addKeyword(ht,"endwhile",TK_ENDWHILE);
	addKeyword(ht,"if",TK_IF);
	addKeyword(ht,"then",TK_THEN);
	addKeyword(ht,"endif",TK_ENDIF);
	addKeyword(ht,"read",TK_READ);
	addKeyword(ht,"write",TK_WRITE);
	addKeyword(ht,"return",TK_RETURN);
	addKeyword(ht,"call",TK_CALL);
	addKeyword(ht,"record",TK_RECORD);
	addKeyword(ht,"endrecord",TK_ENDRECORD);
	addKeyword(ht,"else",TK_ELSE);

	return ht;
}

int hash(char* key)
{
	unsigned long hash = 5381;
	int c;
	while (c = *key++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % HT_SIZE);
}


TK_TYPES findKeyword(HashTable ht, char* key)
{
	int index=hash(key);
	if(ht[index].isvalid==0)
		return NOT_KEYWORD;
	if(strcmp(ht[index].var_name,key)==0)
		return ht[index].type_of_token;
	Node temp=ht[index].next;

	while(temp!=NULL)
	{
		if(strcmp(temp->var_name,key)==0)
			return temp->type_of_token;
		temp=temp->next;
	}
	return NOT_KEYWORD;
}

void addKeyword(HashTable ht, char* key, TK_TYPES type_of_token)
{
	int index=hash(key);
	if(ht[index].isvalid==0)
	{
		ht[index].isvalid=1;
		ht[index].type_of_token=type_of_token;
		strcpy(ht[index].var_name,key);
		return;
	}
	/*if(ht[index].next==NULL)
	{
		ht[index].next=(Node)malloc(sizeof(struct node));
		Node temp=ht[index].next;
		temp->type_of_token=type_of_token;
		strcpy(temp->var_name,key);
		temp->next=NULL;
		return;
	}*/
	Node temp=(Node)malloc(sizeof(struct node));
	temp->type_of_token=type_of_token;
	strcpy(temp->var_name,key);
	temp->next=ht[index].next;
	ht[index].next=temp;
	return;
}

void freeHashTable()
{
	int i;
	Node temp,prev;
	for(i=0;i<HT_SIZE;i++)
	{
		temp=ht[i].next;
		while(temp!=NULL)
		{
			prev=temp;
			temp=prev->next;
			free(prev);
		}
	}
	free(ht);
}