#ifndef _SYMBOLDEF_
#define _SYMBOLDEF_

#include "stackTree.h"

#define F_TABLE_SIZE 13
#define IND_TABLE_SIZE 59
#define REC_TABLE_SIZE 19
#define GLOBAL_TABLE_SIZE 19

typedef struct indEntry{
    int isvalid;
    char name[MAX_LEXEME_SIZE+1];
    TypeList tlist;
    // varTypes vtype;
    int width;
    int offset;
    struct indEntry *next;
}indEntry;

typedef indEntry *IndTable;
typedef indEntry globalEntry;

typedef struct recEntry{
    int isvalid;
    char name[MAX_LEXEME_SIZE+1];
    TypeList tlist;
    //varTypes vtype;
    int width;
    struct recEntry *next;
}recEntry;

typedef recEntry *RecTable;

typedef struct funcEntry
{
    int isvalid;
    char name[MAX_LEXEME_SIZE+1];
    IndTable idtable;
    AstNode input_par;
    AstNode output_par;
    struct funcEntry *next;
}funcEntry;

typedef funcEntry *FuncTable;

#endif