#include "stackTree.h"

extern char *map_nonterminals_to_strings[TOTAL_NUM_NT];
extern char *map_terminals_to_strings[TOTAL_NUM_TOKENS];
extern char *map_vtype_to_strings[4];

//root->addr holds the address of ast subtree
void createAst(TreeNode root)
{
    if(root == NULL){
        printf("root null, returning \n");
        return;
    }
    
    TreeNode temp = root->child;
    /* if(temp == NULL){
        printf("null found termid: %s ruleid = %d, lineno = %d\n", (root->grammar_node->t == 1) ? map_nonterminals_to_strings[root->grammar_node->s.nonterm_type] : map_terminals_to_strings[root->grammar_node->s.term_type], root->rule_id, root->line);
        // printf("ruleid : %d\n", root->rule_id);
        // if(root->rule_id == 32){
        //     printf("garbage s*ala : %d\n", (int) (root->grammar_node == NULL));
        // }
    } */

    switch(root->rule_id)
    {
        //terminals
        case -2:
            //root->addr = NULL;
            // printf("null found termid: %s ruleid = %d, lineno = %d\n", (root->grammar_node->t == 1) ? map_nonterminals_to_strings[root->grammar_node->s.nonterm_type] : map_terminals_to_strings[root->grammar_node->s.term_type], root->rule_id, root->line);
            // printf("returning\n");
            return;
        
        //rules where addr is simply passed on to the parent and no new node is created
        //1st non terminal child
        case 9:
        case 10:
        case 31 ... 35: //rules for stmt
        case 62:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = root->child->addr;
            root->addr->tlist = root->child->addr->tlist;
            return;
        
        //rules where addr of 2nd child passed to parent
        case 14:
        case 41:
        case 43:
        case 61:
        case 86:
        case 87:
        case 90:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = root->child->right->addr;
            return;
        
        //special (taking from 4th child)
        case 5:
        case 6:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = root->child->right->right->right->right->addr;
            return;
        
        //rules giving eps
        case 3:
        case 7:
        case 15:
        case 18:
        case 23:
        case 25:
        case 28:
        case 30:
        case 39:
        case 42:
        case 47:
        case 54:
        case 71:
        case 88:
        case 91:            
            return;

        //mainFunction ===> TK_MAIN stmts TK_END
        case 1:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->child = root->child->right->addr;
            return;
        
        //program ===> otherFunctions mainFunction
        case 0:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root);
            /* root->addr->child = root->child->addr;
            if(root->addr->child == NULL){ //otherfuns is null
                root->addr->child = root->child->right->addr;
            }
            else{  //otherfuns is not null
                root->addr->child->right = root->child->right->addr;
            } */

            //otherfuns node
            root->addr->child = createAstNode(root->child);
            root->addr->child->child = root->child->addr;

            //mainfun node
            root->addr->child->right = root->child->right->addr;
            // root->addr->child->right->child = ;
            return;
            
        //otherFunctions(1) ===> function otherFunctions(2)
        case 2:
        //typeDefinitions(1) ===> typeDefinition typeDefinitions(2)
        case 17:
        //declarations(1) ===> declaration declarations(2)
        case 24:
        //otherStmts(1) ===> stmt otherStmts(2)
        case 29:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = root->child->addr;
            root->addr->right = root->child->right->addr;
            return;

        //moreFields(1) ===> fieldDefinition moreFields(2)
        case 22:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = root->child->addr;
            root->addr->right = root->child->right->addr;
            root->addr->tlist->next = NULL;
            /* if(root->child->right->addr == NULL){
                root->addr->tlist->next = NULL;
            }
            else{
                root->addr->tlist->next = root->child->right->addr->tlist;
            } */
            return;

        //function ===> TK_FUNID input_par output_par TK_SEM stmts TK_END
        case 4:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            //FUNID node
            root->addr = createAstNode(root->child);
            
            //input_par node
            root->addr->child = createAstNode(root->child->right);
            root->addr->child->child = root->child->right->addr;
            root->addr->child->tlist = root->child->right->addr->tlist;
            
            //output_par node
            root->addr->child->right = createAstNode(root->child->right->right);
            root->addr->child->right->child = root->child->right->right->addr;
            root->addr->child->right->tlist = root->child->right->right->addr->tlist;

            //stmts node
            root->addr->child->right->right = root->child->right->right->right->right->addr;
            return;   

        //parameter_list ===> dataType TK_ID remaining_list ///datatype to be handled
        case 8:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child->right);
            root->addr->tlist = (TypeList) malloc(sizeof(struct typeList));  //assign type
            switch (root->child->addr->s.term_type)
            {
                case TK_INT:
                    root->addr->tlist->vtype = intType;
                    break;

                case TK_REAL:
                    root->addr->tlist->vtype = realType;
                    break;

                case TK_RECORDID:
                    root->addr->tlist->vtype = recordType;
                    strncpy(root->addr->recName, root->child->addr->value, MAX_LEXEME_SIZE + 1);
                    break;

                default:
                    root->addr->tlist->vtype = errType;
                    break;
            }
            //inserAtHead of linked list of types
            /* if(root->child->right->right->addr == NULL){
                root->addr->tlist->next = NULL;
            }
            else{
                root->addr->tlist->next = root->child->right->right->addr->tlist;
            } */
            root->addr->tlist->next = NULL;
            root->addr->right = root->child->right->right->addr;
            return;

        //stmts ===> typeDefinitions declarations otherStmts returnStmt
        case 16:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root);
            /* root->addr->child = root->child->addr;
            if(root->addr->child == NULL){ //typedefs is null
                root->addr->child = root->child->right->addr;
                if(root->addr->child == NULL){ //declarations is null
                    root->addr->child = root->child->right->right->addr;
                    if(root->addr->child == NULL){ //otherstmts is null
                        root->addr->child = root->child->right->right->right->addr;
                    }
                    else{  //otherstmts is not null
                        root->addr->child->last->right = root->child->right->right->right->addr;
                    }
                }
                else{ //delclarations is not null
                    root->addr->child->right = root->child->right->right->addr;
                    if(root->addr->child->right == NULL){ //otherstmts is null
                        root->addr->child->right = root->child->right->right->right->addr;
                    }
                    else{  //otherstms is not null
                        root->addr->child->right->right = root->child->right->right->right->addr;
                    }
                }
            }
            else{  //typedefs is not null
                root->addr->child->right = root->child->right->addr;
                if(root->addr->child->right == NULL){ //declarations is null
                    root->addr->child->right = root->child->right->right->addr;
                    if(root->addr->child->right == NULL){ //otherstmts is null
                        root->addr->child->right = root->child->right->right->right->addr;
                    }
                    else{  //otherstmts is not null
                        root->addr->child->right->right = root->child->right->right->right->addr;
                    }
                }
                else{ //delclarations is not null
                    root->addr->child->right->right = root->child->right->right->addr;
                    if(root->addr->child->right->right == NULL){ //otherstmts is null
                        root->addr->child->right->right = root->child->right->right->right->addr;
                    }
                    else{  //otherstms is not null
                        root->addr->child->right->right->right = root->child->right->right->right->addr;
                    }
                }
            } */
            //typedefs node
            root->addr->child = createAstNode(root->child);
            root->addr->child->child = root->child->addr;

            //declarations node
            root->addr->child->right = createAstNode(root->child->right);
            root->addr->child->right->child = root->child->right->addr;

            //otherstmts node
            root->addr->child->right->right = createAstNode(root->child->right->right);
            root->addr->child->right->right->child = root->child->right->right->addr; 
            
            //returnstmts node 
            root->addr->child->right->right->right = createAstNode(root->child->right->right->right);
            root->addr->child->right->right->right->child = root->child->right->right->right->addr;
            return;

        //typeDefinition ===> TK_RECORD TK_RECORDID fieldDefinitions TK_ENDRECORD TK_SEM
        case 19:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child->right);
            root->addr->child = root->child->right->right->addr;
            // root->addr->tlist = root->child->right->right->addr->tlist;  //handling type list
            AstNode tempForList = root->child->right->right->addr;
            TypeList newTlist = (TypeList) malloc(sizeof(struct typeList)), prev, orig;
            newTlist->vtype = tempForList->tlist->vtype;
            strncpy(newTlist->fieldName, tempForList->value, MAX_LEXEME_SIZE+1);
            newTlist->next = NULL;
            prev = newTlist;
            orig = newTlist;
            newTlist = newTlist->next;
            tempForList = tempForList->right;
            while(tempForList!=NULL){
                newTlist = (TypeList)malloc(sizeof(struct typeList));
                prev->next = newTlist;
                newTlist->vtype = tempForList->tlist->vtype;
                strncpy(newTlist->fieldName, tempForList->value, MAX_LEXEME_SIZE+1);
                newTlist->next = NULL;
                prev = newTlist;
                newTlist = newTlist->next;
                tempForList = tempForList->right;
            }
            root->addr->tlist = orig;
            return;

        //fieldDefinitions ===> fieldDefinition(1) fieldDefinition(2) moreFields
        case 20:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = root->child->addr;
            root->addr->right = root->child->right->addr;
            root->addr->right->right = root->child->right->right->addr;

            //create type linked list using fd etc
            root->addr->tlist = root->child->addr->tlist;
            // root->addr->tlist->next = root->child->right->addr->tlist;
            /* if(root->child->right->right->addr == NULL){
                root->addr->tlist->next->next = NULL;
            }
            else{
                root->addr->tlist->next->next = root->child->right->right->addr->tlist;
            } */
            return;

        //fieldDefinition ===> TK_TYPE primitiveDatatype TK_COLON TK_FIELDID TK_SEM   ///handle datatype!!!
        case 21:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child->right->right->right);
            root->addr->tlist = (TypeList) malloc(sizeof(struct typeList));  //assign type
            strncpy(root->addr->tlist->fieldName, root->addr->value, MAX_LEXEME_SIZE + 1);
            root->addr->tlist->next = NULL;
            switch (root->child->right->addr->s.term_type)
            {
                case TK_INT:
                    root->addr->tlist->vtype = intType;
                    break;

                case TK_REAL:
                    root->addr->tlist->vtype = realType;
                    break;

                default:
                    root->addr->tlist->vtype = errType;
                    break;
            }
            return;
        
        //declaration ===> TK_TYPE dataType TK_COLON TK_ID global_or_not TK_SEM    ///handle type
        case 26:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child->right->right->right);
            if(root->child->right->right->right->right->rule_id == 27){
                root->addr->globalFlg = 1;
            }
            root->addr->tlist = (TypeList) malloc(sizeof(struct typeList));  //assign type
            root->addr->tlist->next = NULL;
            switch (root->child->right->addr->s.term_type)
            {
                case TK_INT:
                    root->addr->tlist->vtype = intType;
                    break;

                case TK_REAL:
                    root->addr->tlist->vtype = realType;
                    break;

                case TK_RECORDID:
                    root->addr->tlist->vtype = recordType;
                    // root->addr->recName = (char *) malloc(sizeof(char) * MAX_LEXEME_SIZE+1);
                    strncpy(root->addr->recName, root->child->right->addr->value, MAX_LEXEME_SIZE + 1);
                    break;

                default:
                    root->addr->tlist->vtype = errType;
                    break;
            }
            return;

        //assignmentStmt ===> singleOrRecId TK_ASSIGNOP arithmeticExpression TK_SEM  ///handle types
        case 36:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child->right);
            root->addr->child = root->child->addr;
            root->addr->child->right = root->child->right->right->addr;
            return;

        //singleOrRecId ===> TK_ID new24
        case 37:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->child = root->child->right->addr;
            return;

        //funCallStmt ===> outputParameters TK_CALL TK_FUNID TK_WITH TK_PARAMETERS inputParameters TK_SEM
        case 40:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root);
            //outputparams node
            root->addr->child = createAstNode(root->child);
            root->addr->child->child = root->child->addr;
            root->addr->child->tlist = root->child->addr->tlist;
            
            //funid node
            root->addr->child->right = createAstNode(root->child->right->right);
            root->addr->child->right->child = root->child->right->right->addr;
            
            //inputparameters node
            root->addr->child->right->right = createAstNode(root->child->right->right->right->right->right);
            root->addr->child->right->right->child = root->child->right->right->right->right->right->addr;
            root->addr->child->right->right->tlist = root->child->right->right->right->right->right->addr->tlist;
            
            return;

        //iterativeStmt ===> TK_WHILE TK_OP booleanExpression TK_CL stmt otherStmts TK_ENDWHILE
        case 44:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root);
            root->addr->child = root->child->right->right->addr;
            /* //root->addr->right = root->child->right->right->addr;
            root->addr->right->right = root->child->right->right->right->right->addr;
            root->addr->right->right->right = root->child->right->right->right->right->right->addr; */
            root->addr->child->right = root->child->right->right->right->right->addr;
            root->addr->child->right->right = root->child->right->right->right->right->right->addr;
            return;

        //conditionalStmt ===> TK_IF TK_OP booleanExpression TK_CL TK_THEN stmt otherStmts elsePart
        case 45:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            //conditionalStmt node
            root->addr = createAstNode(root);
            root->addr->child = root->child->right->right->addr;

            //then stmts are siblings of boolEx
            root->addr->child->right = createAstNode(root->child->right->right->right->right->right);
            root->addr->child->right->child = root->child->right->right->right->right->right->addr;
            root->addr->child->right->child->right = root->child->right->right->right->right->right->right->addr;

            //else statements are also siblings of boolEx
            root->addr->child->right->right = root->child->right->right->right->right->right->right->right->addr;
            return;

        //elsePart ===> TK_ELSE stmt otherStmts TK_ENDIF
        case 46:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->child = root->child->right->addr;
            root->addr->child->right = root->child->right->right->addr;
            return;

        //ioStmt ===> TK_READ TK_OP singleOrRecId TK_CL TK_SEM
        case 48:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->child = root->child->right->right->addr;
            return;

        //ioStmt ===> TK_WRITE TK_OP allVar TK_CL TK_SEM
        case 49:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->child = root->child->right->right->addr;
            return;     

        //allVar ===> TK_ID newVar
        case 50:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->child = root->child->right->addr;
            return;

        //arithmeticExpression ===> term expPrime
        case 55:
        //term ===> factor termPrime
        case 58:
            createAst(temp);
            temp->right->inh = temp->addr;
            createAst(temp->right);
            root->addr = temp->right->syn;
            return;

        //expPrime(1) ===> lowPrecedenceOperators term expPrime(2)
        case 56:
        //termPrime(1) ===> highPrecedenceOperators factor termPrime(2)
        case 59:
            createAst(temp);
            createAst(temp->right);
            root->child->right->right->inh = temp->addr;
            temp->addr->child = root->inh;
            temp->addr->child->right = root->child->right->addr;
            createAst(root->child->right->right);
            root->syn = root->child->right->right->syn;
            return;

        //expPrime ===> eps
        case 57:
        //termPrime ===> eps
        case 60:
            root->syn = root->inh;
            return;


        //all ===> TK_ID temp
        case 69:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->child = root->child->right->addr;
            return;

        //booleanExpression(1) ===> TK_OP booleanExpression(2) TK_CL logicalOp TK_OP booleanExpression(3) TK_CL
        case 72:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = root->child->right->right->right->addr;
            root->addr->child = root->child->right->addr;
            root->addr->child->right = root->child->right->right->right->right->right->addr;
            return;

        //booleanExpression ===> var(1) relationalOp var(2)
        case 73:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = root->child->right->addr;
            root->addr->child = root->child->addr;
            root->addr->child->right = root->child->right->right->addr;
            return;

        //booleanExpression(1) ===> TK_NOT TK_OP booleanExpression(2) TK_CL
        case 74:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->child = root->child->right->right->addr;
            return;
        
        //idList ===> TK_ID more_ids
        case 89:
            while(temp!=NULL){
                createAst(temp);
                temp=temp->right;
            }
            root->addr = createAstNode(root->child);
            root->addr->right = root->child->right->addr;
            return;
        
        //Nonterms to a single term
        
        //primitiveDatatype ===> TK_INT
        case 11:

        //primitiveDatatype ===> TK_REAL
        case 12:

        //highPrecedenceOperators ===> TK_MUL
        case 63:

        //highPrecedenceOperators ===> TK_DIV
        case 64:

        //lowPrecedenceOperators ===> TK_PLUS
        case 65:

        //lowPrecedenceOperators ===> TK_MINUS
        case 66:

        //var ===> TK_ID                           ///symbol table lookup for type?
        case 75:

        //logicalOp ===> TK_AND
        case 78:

        //logicalOp ===> TK_OR
        case 79:

        //relationalOp ===> TK_LT
        case 80:

        //relationalOp ===> TK_LE
        case 81:

        //relationalOp ===> TK_EQ
        case 82:

        //relationalOp ===> TK_GT
        case 83:

        //relationalOp ===> TK_GE
        case 84:

        //relationalOp ===> TK_NE
        case 85:
            root->addr = createAstNode(root->child);
            //printf("Test: %s\n", root->addr->value);
            return;

        //type handled here
        //allVar ===> TK_NUM
        case 51:

        //allVar ===> TK_RNUM
        case 52:

        //all ===> TK_NUM
        case 67:

        //all ===> TK_RNUM
        case 68:

        //var ===> TK_NUM
        case 76:

        //var ===> TK_RNUM
        case 77:
            root->addr = createAstNode(root->child);
            root->addr->tlist = (TypeList) malloc(sizeof(struct typeList));
            root->addr->tlist->next = NULL;
            switch (root->child->grammar_node->s.term_type)
            {
                case TK_NUM:
                    root->addr->tlist->vtype = intType;
                    break;
            
                case TK_RNUM:
                    root->addr->tlist->vtype = realType;
                    break;

                default:
                    root->addr->tlist->vtype = errType;
                    break;
            }
            //printf("Test: %s\n", root->addr->value);
            return;

        //2nd child terminal
        //constructedDatatype ===> TK_RECORD TK_RECORDID                           ///symbol table lookup for type?
        case 13:

        //global_or_not ===> TK_COLON TK_GLOBAL
        case 27:
            root->addr = createAstNode(root->child->right);
            return;

        //new24 ===> TK_DOT TK_FIELDID                           ///symbol table lookup for type?
        case 38:
        
        //temp ===> TK_DOT TK_FIELDID
        case 70:

        //newVar ===> TK_DOT TK_FIELDID
        case 53:
            root->addr = createAstNode(root->child->right);
            //root->addr->tlist = createTypeList();
            return;

    }
}

void printAst(AstNode root){
    if(root == NULL){
        return;
    }
    if(root->t == NON_TERM){
        printf("Non-term: %-25s", map_nonterminals_to_strings[root->s.nonterm_type]);
        if(root->tlist != NULL){
            TypeList tempList = root->tlist;
            // printf("Term: %s\tValue: %s\t", map_terminals_to_strings[root->s.term_type], root->value);
            while(tempList != NULL){
                printf("Type: %-25s\t", map_vtype_to_strings[tempList->vtype]);
                tempList = tempList->next;
            }
        }
        printf("\n");
    }
    else{
        // printf("Term: %s\tValue: %s\n", map_terminals_to_strings[root->s.term_type], root->value);
        if(root->tlist != NULL){
            TypeList tempList = root->tlist;
            printf("Line no: %-25d\tTerm: %-25s\tValue: %-25s\t", root->line_number, map_terminals_to_strings[root->s.term_type], root->value);
            while(tempList != NULL){
                printf("Type: %-25s\t", map_vtype_to_strings[tempList->vtype]);
                tempList = tempList->next;
            }
            printf("\n");            
        }
        else{
            printf("Line no: %-25d\tTerm: %-25s\tValue: %-25s\n", root->line_number, map_terminals_to_strings[root->s.term_type], root->value);
        }
    }
    AstNode temp = root->child;
    while(temp!=NULL){
        printAst(temp);
        temp = temp->right;
    }
}