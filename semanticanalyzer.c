#include "semanticanalyzer.h"

extern FuncTable ftable[F_TABLE_SIZE];
extern char* map_vtype_to_strings[4];
extern int errflag;

void semanticCheck(AstNode root, AstNode astRoot, char *funcScope){
    if(root == NULL){
        return;
    }
    AstNode ch = root->child;
    while(ch != NULL){
        semanticCheck(ch, astRoot, funcScope);
        ch = ch->right;
    }

    struct funcEntry *temp;
    TypeList tl;
    TypeList sl;
    AstNode tempAst, stmt;
    AstNode tempAstIter;
    int updated = 0;                // 0 ==> variable not updated, 1 ==> variable updated
    int definedPreviously = 0;
    int funcBeingCalledIndex;

    if(root->t == NON_TERM){
        switch(root->s.nonterm_type){
            case funCallStmt:
                //first check if this function is defined beforehand
                //program -> otherFunctions -> function linked list
                tempAst = astRoot->child->child;
                char *funcBeingCalled = root->child->right->value;
                if(strcmp(funcScope, funcBeingCalled) == 0){
                    errflag = 1;
                    printf("Line %d: self-recursive call to function <%s>.\n", root->child->right->line_number, funcBeingCalled);
                    break;
                }
                while(tempAst != NULL && strcmp(tempAst->value, funcScope) != 0){   //iterate till you reach current func's definition
                    if(strcmp(tempAst->value, funcBeingCalled) == 0){
                        definedPreviously = 1;
                        break;
                    }
                    tempAst = tempAst->right;
                }
                if(definedPreviously == 0){
                    errflag = 1;
                    printf("Line %d: undefined function <%s>.\n", root->child->right->line_number, funcBeingCalled);
                    break;
                }

                temp = funcTableLookup(funcBeingCalled);
                if(temp == NULL){
                    errflag = 1;
                    printf("Line %d: undefined function <%s>.\n", root->child->right->line_number, funcBeingCalled);
                    break;
                }
                //output params ka tlist uske pehle child ke paas hai aur Gangadhar hi Shaktimaan hai
                tl = root->child->child->tlist;
                sl = temp->output_par->tlist;
                while(1){
                    // printf("--------------%d\t%d", tl->vtype, sl->vtype);
                    //successful completion
                    if(tl == NULL && sl == NULL){
                        break;
                    }
                    //Different number of parameters than expected
                    if((tl == NULL && sl != NULL) || (tl != NULL && sl == NULL)){
                        errflag = 1;
                        printf("Line %d: The number of output parameters at function call <%s> is incorrect.\n", root->child->right->line_number, funcBeingCalled);
                        break;
                    }
                    //Checking type
                    if(tl->vtype != sl->vtype && tl->vtype != errType && sl->vtype != errType){
                        errflag = 1;
                        printf("Line %d: The type <%s> of the variable does not match with the type <%s> of the formal parameter.\n", root->child->right->line_number, map_vtype_to_strings[tl->vtype], map_vtype_to_strings[sl->vtype]);
                    }
                    tl = tl->next;
                    sl = sl->next;
                }
                //Number and type of input parameters
                tl = root->child->right->right->child->tlist;
                sl = temp->input_par->tlist;
                while(1){
                    //successful completion
                    if(tl == NULL && sl == NULL){
                        break;
                    }
                    //Different number of parameters than expected
                    if((tl == NULL && sl != NULL) || (tl != NULL && sl == NULL)){
                        errflag = 1;
                        printf("Line %d: The number of input parameters at function call <%s> is incorrect.\n", root->child->right->line_number, funcBeingCalled);
                        break;
                    }
                    //Checking type
                    if(tl->vtype != sl->vtype && tl->vtype != errType && sl->vtype != errType){
                        errflag = 1;
                        printf("Line %d: Input parameter type mismatch\n", root->child->right->line_number);
                    }
                    tl = tl->next;
                    sl = sl->next;
                }
                break;

            case iterativeStmt:   //check if condition variables get updated in an iteration, else infinite loop.
                /*
                    traverse the ast of boolean expression and call the helper function on evey variable
                */
                tempAstIter = root->child;
                while(tempAstIter->right != NULL){
                    tempAstIter = tempAstIter->right;
                }
                if(traverseBooleanAst(root->child, root) == 0){
                    if(tempAstIter->t == NON_TERM){
                        errflag = 1;
                        printf("lines %d-%d: None of the variables participating in the iterations of the while loop are updated\n", root->child->right->line_number, tempAstIter->child->line_number);
                    }
                    else{
                        errflag = 1;
                        printf("lines %d-%d: None of the variables participating in the iterations of the while loop are updated\n", root->child->right->line_number, tempAstIter->line_number);
                    }                    
                }
                break;
        }
    }
    else{
        AstNode funAssign;
        if(root->s.term_type == TK_FUNID && root->child != NULL){
            tempAst = root->child->right->child;                                //output_par node
            while(tempAst != NULL){
                updated = 0;
                stmt = root->child->right->right->child->right->right->child;   //stmts linked list
                while(stmt != NULL){
                    if(stmt->t == TERM && (stmt->s.term_type == TK_ASSIGNOP || stmt->s.term_type == TK_READ)){
                        if(strcmp(tempAst->value, stmt->child->value) == 0){
                            
                            //useless!!!
                            if(tempAst->child != NULL && stmt->child->child!=NULL && strcmp(tempAst->child->value, stmt->child->child->value)==0){
                                updated = 1;
                                break;
                            }
                            else if(tempAst->child == NULL && stmt->child->child == NULL){
                                updated = 1;
                                break;
                            }
                            updated = 1;
                            break;
                        }
                    }
                    else if(stmt->t == NON_TERM && stmt->s.nonterm_type == funCallStmt){
                        funAssign = stmt->child->child;      //outpar list of this func call
                        while(funAssign != NULL){
                            if(!strcmp(tempAst->value, funAssign->value)){
                                updated = 1;
                                break;
                            }
                            funAssign = funAssign->right;
                        }                        
                    }
                    /* if(updated){
                        break;
                    } */
                    stmt = stmt->right;
                }
                if(!updated){
                    errflag = 1;
                    printf("Line %d: Return parameter <%s> for the <%s> function is not assigned a value\n", root->line_number, tempAst->value, root->value);
                }
                tempAst = tempAst->right;
            }
            
            //check if return type matches output param list
            AstNode tempOutPar = root->child->right->child;                                //output_par node
            AstNode retOutPar;
            if(strcmp(funcScope, "_main") != 0){
                retOutPar = root->child->right->right->child->right->right->right->child; //returnStmt node
            }
            else{
                retOutPar = root->child->child->right->right->right->child;  //returnStmt node for main func
                if(retOutPar != NULL){
                    errflag = 1;
                    printf("Line %d: Main function cannot return parameters\n", retOutPar->line_number);
                }
                return;
            }
            while(1){
                if(tempOutPar == NULL && retOutPar == NULL){
                    break;
                }
                if((tempOutPar == NULL && retOutPar != NULL) || (tempOutPar != NULL && retOutPar == NULL)){
                    errflag = 1;
                    printf("Line %d: Number of returned parameters does not match with the expected number of output parameters\n", root->line_number);
                    break;
                }
                if(strcmp(tempOutPar->value, retOutPar->value) != 0){
                    errflag = 1;
                    printf("Line %d: Returned parameter <%s> does not match with the formal output parameter <%s>\n", retOutPar->line_number, retOutPar->value, tempOutPar->value);
                }
                tempOutPar = tempOutPar->right;
                retOutPar = retOutPar->right;
            }
        }
    }
}

int iterUpdateHelper(AstNode variable, AstNode iterStmt){ //returns 1 if the variable is updated, else 0
    AstNode stmt = iterStmt->child->right;                //go to stmt linklist
    while(stmt != NULL){
        if(stmt->t == TERM && stmt->s.term_type == TK_ASSIGNOP){    //check assignment stmts
            if(strcmp(stmt->child->value, variable->value) == 0){   //if variable updated (LHS of assignment)
                
                //check for situations like a.b etc useless!!!
                if(stmt->child->child != NULL && variable->child != NULL && strcmp(stmt->child->child->value, variable->child->value) == 0){
                    return 1;
                }
                else if(stmt->child->child == NULL && variable->child == NULL){
                    return 1;
                }
                return 1;
            }
        }
        stmt = stmt->right;
    }
    if(stmt == NULL){
        return 0;
    }
}

int traverseBooleanAst(AstNode boolAst, AstNode root) {
    // AstNode temp = boolAst->child;
    switch (boolAst->s.term_type)
    {
        case TK_LT:
        case TK_LE:
        case TK_GT:
        case TK_GE:
        case TK_EQ:
        case TK_NE:
            return iterUpdateHelper(boolAst->child, root) || iterUpdateHelper(boolAst->child->right, root);
        
        case TK_AND:
        case TK_OR:
            return traverseBooleanAst(boolAst->child, root) || traverseBooleanAst(boolAst->child->right, root);

        case TK_NOT:
            return traverseBooleanAst(boolAst->child, root);
    
        default:
            errflag = 1;
            printf("Serious error in traversebooleanast()\n");
            break;
    }
}

void semanticAnalyzer(AstNode root){
    AstNode temp = root->child->child;
    while(temp != NULL){
        semanticCheck(temp, root, temp->value);
        temp = temp->right;        
    }
    AstNode mainFunc = root->child->right;
    semanticCheck(mainFunc, root, mainFunc->value);
}