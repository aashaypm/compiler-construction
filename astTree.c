#include "stackTree.h"
int astMem = 0; //total bytes take by ast
int astNodes = 0; //number of nodes in ast
TypeList createTypeList(varTypes vtype)
{
    TypeList tlist = (TypeList) malloc(sizeof(struct typeList));
    tlist->vtype = vtype;
    tlist->next = NULL;
    return tlist;
}


AstNode createAstNode(TreeNode t)
{
    AstNode a = (AstNode)malloc(sizeof(struct astNode));
    a->parent = NULL;
    a->right = NULL;
    a->child = NULL;
    a->t = t->grammar_node->t;
    if(a->t == 0){
        a->s.term_type = t->grammar_node->s.term_type;
    }
    else if (a->t == 1) {
        a->s.nonterm_type = t->grammar_node->s.nonterm_type;
    }
    strncpy(a->value, t->value, MAX_FLOAT_LEN+1);
    a->globalFlg = 0;
    a->line_number = t->line;
    a->tlist = NULL;
    a->recName[0] = '\0';
    //a->initialization_line = -1;
    a->intreg = -1;
    for(int r = 0; r < RECORD_REGS; r++){
        a->regLocations[r] = -1;
    }
    astMem+=sizeof(a);
    astNodes += 1;
    return a;
}

