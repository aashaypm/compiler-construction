/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#ifndef _LEXDEF_
#define _LEXDEF_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define TOTAL_NUM_TOKENS 55	//number of token classes
#define BUF_SIZE 1024		//size of buffer
#define MAX_LEXEME_SIZE 30	//maximum length of lexeme
#define NUM_KEYWORDS 24		//number of keywords
#define MAX_KEYWORD_SIZE 11	//maximum length of keyword
#define HT_SIZE 59			//size of hash table
#define MAX_INT_LEN 11		//maximum length of an integer
#define MAX_FLOAT_LEN 42	//maximum length of a real number

//enum for token ids
typedef enum{
	TK_ASSIGNOP,
	TK_COMMENT,
	TK_FIELDID,
	TK_ID,
	TK_NUM,
	TK_RNUM,
	TK_FUNID,
	TK_RECORDID,
	TK_WITH,
	TK_PARAMETERS,
	TK_END,
	TK_WHILE,
	TK_TYPE,
	TK_MAIN,
	TK_GLOBAL,
	TK_PARAMETER,
	TK_LIST,
	TK_SQL,
	TK_SQR,
	TK_INPUT,
	TK_OUTPUT,
	TK_INT,
	TK_REAL,
	TK_COMMA,
	TK_SEM,
	TK_COLON,
	TK_DOT,
	TK_ENDWHILE,
	TK_OP,
	TK_CL,
	TK_IF,
	TK_THEN,
	TK_ENDIF,
	TK_READ,
	TK_WRITE,
	TK_RETURN,
	TK_PLUS,
	TK_MINUS,
	TK_MUL,
	TK_DIV,
	TK_CALL,
	TK_RECORD,
	TK_ENDRECORD,
	TK_ELSE,
	TK_AND,
	TK_OR,
	TK_NOT,
	TK_LT,
	TK_LE,
	TK_EQ,
	TK_GT,
	TK_GE,
	TK_NE,
	TK_EPS,
	TK_DOL,
	NOT_KEYWORD,			//for eof
	NOT_TOKEN,				//erraneous token
	UNK_SYMB
}TK_TYPES;

typedef struct lexeme* Lexeme;

typedef struct {
	TK_TYPES type_of_token;
	int line_num;
	char value[MAX_FLOAT_LEN];
}tokenInfo; 


typedef struct node{
	TK_TYPES type_of_token;
	char var_name[MAX_KEYWORD_SIZE];
	struct node* next;
}* Node;

typedef struct{
	int isvalid;
	TK_TYPES type_of_token;
	char var_name[MAX_KEYWORD_SIZE];
	Node next;
}htentry;


typedef tokenInfo* TokenInfo;
typedef htentry* HashTable;

//hashtable fuctions
HashTable initializeHashTable();
void addKeyword(HashTable ht, char* key, TK_TYPES type_of_token);
int hash(char* key);
TK_TYPES findKeyword(HashTable ht, char* key);
void freeHashTable();
#endif