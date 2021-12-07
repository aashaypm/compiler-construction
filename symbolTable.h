#ifndef _SYMBTABLE_
#define _SYMBTABLE_

#include "symbolDef.h"
//func table fuctions
void initializeFuncTable();
void addFunction(AstNode funcNode);
void printFunctionTable();
int hashFunc(char *key);

//TK_TYPES findKeyword(HashTable ht, char* key);
void freeFuncTable();
indEntry* symbolTableLookup(char* funcName, char* varName);
indEntry* globalTableLookup(char* varName);
recEntry* recordTableLookup(char* recName);
funcEntry* funcTableLookup(char* funcName);
void printGlobalTable();
void printRecordTable();
void printSymbolTable();
void printFuncMems();
void populateSymbolTables(AstNode root);

#endif