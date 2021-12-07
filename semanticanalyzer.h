#ifndef _SEMANAL_
#define _SEMANAL_

#include "symbolTable.h"

void semanticCheck(AstNode root, AstNode astRoot, char *funcScope);
int traverseBooleanAst(AstNode boolAst, AstNode root);
void propagateType(AstNode root);
void semanticAnalyzer(AstNode root);

#endif