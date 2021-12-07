/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#ifndef _PARSER_
#define _PARSER_
#include "parserDef.h"
#include "stackTree.h"

void loadGrammar();
void computeFirstSet();
void computeFollowSet();
TreeNode parse(FILE *fp);
void createParsingTable();
void printParseTable();
void printTree(TreeNode root);
void freeParseTree(TreeNode root);
void freeGrammar();
#endif