#include "symbolTable.h"
int errflag = 0;

void propagateTypeFunc(AstNode root, char *fname)
{
    if(root == NULL)
        return;
    if(root ->t == NON_TERM && (root->s.nonterm_type == typeDefinitions || root->s.nonterm_type == declarations))
        return;

    AstNode temp = root->child;

    while(temp!=NULL)
    {
        propagateTypeFunc(temp, fname);
        temp = temp->right;
    }
    indEntry* term;
    TypeList checkchild;
    TypeList rec1fields,rec2fields;
    int nooffields=0;

    if(root->t == TERM)
    {
        switch (root->s.term_type)
        {
            case TK_ID:
                term = symbolTableLookup(fname, root->value);
                
                //variable not found after lookup
                if(term == NULL)
                {
                    printf("Line %d: Undeclared variable %s\n",root->line_number,root->value);
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    root->tlist->vtype = errType;
                    errflag = 1;
                    return;
                }

                //if it is not a field from a record
                if(root->child == NULL)
                {
                    root->tlist = term -> tlist;
                }

                //if it a field from a record
                else if(root->child != NULL)
                {
                    checkchild = term -> tlist;
                    while (checkchild != NULL)
                    {
                        //check which fieldname matches from the declared record fields
                        if(strcmp(checkchild->fieldName, root->child->value) == 0)
                        {
                            root->tlist = (TypeList)malloc(sizeof(struct typeList));
                            root->tlist->next = NULL;
                            root->tlist->vtype = checkchild->vtype;

                            //root->child->tlist = (TypeList)
                            //root->child->tlist->vtype = checkchild->vtype;
                            strncpy(root->tlist->fieldName, checkchild->fieldName, MAX_LEXEME_SIZE+1);
                            return;
                        }
                        checkchild = checkchild->next;
                    }
                    printf("Line %d: Record %s does not have fieldname %s\n", root->child->line_number, term->name, root->child->value);
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    strncpy(root->tlist->fieldName, root->child->value, MAX_LEXEME_SIZE+1);
                    root->tlist->vtype = errType;
                    errflag = 1;
                }
                
                break;
                
            
            case TK_PLUS:
            case TK_MINUS:
                if(root->child->tlist->vtype == errType || root->child->right->tlist->vtype == errType)
                {
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    root->tlist->vtype = errType;
                }

                //if they are records
                else if(root->child->tlist->next != NULL && root->child->right->tlist->next !=NULL)
                {
                    rec1fields = root->child->tlist;
                    rec2fields = root->child->right->tlist;

                    while(rec1fields!=NULL && rec2fields!=NULL)
                    {
                        if(rec1fields->vtype != rec2fields->vtype)
                        {
                            printf("Line %d: Arithmetic operation %s on variables of incompatible record types\n", root->child->line_number, root->value);
                            errflag = 1;
                            root->tlist = (TypeList)malloc(sizeof(struct typeList));
                            root->tlist->next = NULL;
                            root->tlist->vtype = errType;
                            return;
                        }
                        rec1fields = rec1fields -> next;
                        rec2fields = rec2fields -> next;
                    }
                    if(rec1fields!=NULL || rec2fields != NULL)
                    {
                        printf("Line %d: Arithmetic operation %s on variables of incompatible record types\n", root->child->line_number, root->value);
                        errflag = 1;
                        root->tlist = (TypeList)malloc(sizeof(struct typeList));
                        root->tlist->next = NULL;
                        root->tlist->vtype = errType;
                        return;
                    }
                    root -> tlist = root -> child -> tlist;
                    root ->line_number = root -> child -> line_number;
                }

                //one of them is a record but the other isn't
                else if(root->child->tlist->next != NULL || root->child->right->tlist->next !=NULL)
                {
                    printf("Line %d: Incompatible addition or subtraction of a scalar and a record \n",root->child->line_number);
                    errflag = 1;
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    root->tlist->vtype = errType;
                    return;
                }

                //both aren't records
                else
                {
                    if(root->child->tlist->vtype == realType || root->child->right->tlist->vtype == realType)
                    {
                        root->tlist = (TypeList)malloc(sizeof(struct typeList));
                        root->tlist->next = NULL;
                        root->tlist->vtype = realType;
                    }
                    else
                    {
                        root->tlist = (TypeList)malloc(sizeof(struct typeList));
                        root->tlist->next = NULL;
                        root->tlist->vtype = intType;
                    }
                    root ->line_number = root -> child -> line_number;
                }
                break;

            case TK_MUL:
            case TK_DIV:
                if(root->child->tlist->vtype == errType || root->child->right->tlist->vtype == errType)
                {
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    root->tlist->vtype = errType;
                }

                //if they are records
                else if(root->child->tlist->next != NULL && root->child->right->tlist->next !=NULL)
                {
                    printf("Line %d: Cannot multiply or divide two records\n",root->child->line_number);
                    errflag = 1;
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    root->tlist->vtype = errType;
                    return;
                }

                //one of them is a record but the other isn't
                else if(root->child->tlist->next != NULL)
                {
                    if(root->child->right->tlist->vtype == intType)
                    {
                        root->tlist = root->child->tlist;
                    }
                    else
                    {
                        rec1fields = root->child->tlist;
                        root->tlist = (TypeList) malloc(sizeof(struct typeList));
                        rec2fields = root->tlist;
                        while(rec1fields!=NULL)
                        {
                            nooffields+=1;
                            rec1fields = rec1fields -> next;
                        }
                        root->tlist->vtype = realType;
                        nooffields--;

                        while(nooffields>0)
                        {
                            rec2fields->next = (TypeList) malloc(sizeof(struct typeList));
                            rec2fields->next->vtype = realType;
                            rec2fields = rec2fields ->next;
                            rec2fields -> next = NULL;
                            nooffields--;
                        }
                    }
                    root ->line_number = root -> child -> line_number;
                    return;
                }
                else if(root->child->right->tlist->next !=NULL)
                {
                    if(root->child->tlist->vtype == intType)
                    {
                        root->tlist = root->child->right->tlist;
                    }
                    else
                    {
                        rec1fields = root->child->right->tlist;
                        root->tlist = (TypeList) malloc(sizeof(struct typeList));
                        rec2fields = root->tlist;
                        while(rec1fields!=NULL)
                        {
                            nooffields+=1;
                            rec1fields = rec1fields -> next;
                        }
                        root->tlist->vtype = realType;
                        nooffields--;

                        while(nooffields>0)
                        {
                            rec2fields->next = (TypeList) malloc(sizeof(struct typeList));
                            rec2fields->next->vtype = realType;
                            rec2fields = rec2fields ->next;
                            rec2fields -> next = NULL;
                            nooffields--;
                        }
                    }
                    root ->line_number = root -> child -> line_number;
                    return;
                }
                //both aren't records
                else
                {
                    if(root->child->tlist->vtype == realType || root->child->right->tlist->vtype == realType)
                    {
                        root->tlist = (TypeList)malloc(sizeof(struct typeList));
                        root->tlist->next = NULL;
                        root->tlist->vtype = realType;
                    }
                    else
                    {
                        root->tlist = (TypeList)malloc(sizeof(struct typeList));
                        root->tlist->next = NULL;
                        root->tlist->vtype = intType;
                    }
                    root ->line_number = root -> child -> line_number;
                }
                break;

            case TK_LT:
	        case TK_LE:
	        case TK_EQ:
	        case TK_GT:
	        case TK_GE: 
            case TK_NE:
                if(root->child->s.term_type == TK_ID)
                {
                    if(root->child->tlist->next!=NULL)
                    {
                        printf("Line %d: Cannot use logical operators on variable %s of type record\n",root->child->line_number, root->child->value);
                        root->tlist = (TypeList)malloc(sizeof(struct typeList));
                        root->tlist->next = NULL;
                        root->tlist->vtype = errType;
                        errflag = 1; 
                        return;   
                    }
                }
                if(root->child->right->s.term_type == TK_ID)
                {
                    if(root->child->right->tlist->next!=NULL)
                    {
                        printf("Line %d: Cannot use logical operators on variable %s of type record\n",root->child->line_number, root->child->value);
                        root->tlist = (TypeList)malloc(sizeof(struct typeList));
                        root->tlist->next = NULL;
                        root->tlist->vtype = errType;
                        errflag = 1;
                        return; 
                    }
                }
                root ->line_number = root -> child -> line_number;                
                break;

            /* case TK_READ:
            case TK_WRITE:
                if(root->child->tlist->next!=NULL)
                {
                    printf("Line %d: Cannot perform I/O operations on variable %s of record type\n",root->child->line_number, root->child->value);
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    root->tlist->vtype = errType;
                    errflag = 1;
                    return; 
                }
                root ->line_number = root -> child -> line_number;
                break; */

            case TK_ASSIGNOP:
                rec1fields = root->child->tlist;
                rec2fields = root->child->right->tlist;
                // printf("Printing tlist in typechecker- rec1: %d, rec2: %d\n", rec1fields->vtype, rec2fields->vtype);
                if(rec1fields->vtype == errType || rec2fields->vtype == errType)
                {
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    root->tlist->vtype = errType;
                }
                while(rec1fields!=NULL && rec2fields!=NULL)
                {
                    if(rec1fields->vtype==intType && rec2fields->vtype==realType)
                    {
                        printf("Line %d: Assignment operation to variable %s incompatible with the type of expression\n", root->child->line_number, root->child->value);
                        errflag = 1;
                        root->tlist = (TypeList)malloc(sizeof(struct typeList));
                        root->tlist->next = NULL;
                        root->tlist->vtype = errType;
                        return;
                    }
                    rec1fields = rec1fields -> next;
                    rec2fields = rec2fields -> next;    
                }
                if(rec1fields!=NULL || rec2fields != NULL)
                {
                    printf("Line %d: Assignment operation to variable %s incompatible with the type of expression\n", root->child->line_number, root->child->value);
                    errflag = 1;
                    root->tlist = (TypeList)malloc(sizeof(struct typeList));
                    root->tlist->next = NULL;
                    root->tlist->vtype = errType;
                    return;
                }
                break;
        
            default:
                break;
        }
    }
}

void propagateType(AstNode root)
{
    //AstNodetemp
    if(root->child->child != NULL)
    {
        AstNode temp = root->child->child;
        while(temp!=NULL)
        {
            propagateTypeFunc(temp,temp->value);
            temp = temp -> right;
        }
    }
    propagateTypeFunc(root->child->right, root->child->right->value);
    return;
    /*AstNode funcNode = root->child->child, temp;
    while(funcNode!=NULL)
    {
        //funid - input par - output par - stmts - typedefinitions - declarations - otherstmts
        temp = funcNode->child->right->right->child->right->right;
        propagateTypeFunc(temp, funcNode->value);
        funcNode = funcNode -> right;
    }
    
    //tk_main - stmts - typedefinitions - individual definition (has recordid)  children are fieldids
    funcNode = root->child->right;
    
    //main - stmts - typedefinitions - declarations - otherstmts
    temp = funcNode->child->child->right->right;
    
    propagateTypeFunc(temp, funcNode->value);*/
}