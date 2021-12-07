#include "codegen.h"

#define OPERATOR_LENGTH 5

int jumpid = 0;
/* char* reglist[16]={
    "r0w",
    "r1w",
    "r2w",
    "r3w",
    "r4w",
    "r5w",
    "r6w",
    "r7w",
    "r8w",
    "r9w",
    "r10w",
    "r11w",
    "r12w",
    "r13w",
    "r14w",
    "r15w"
}; */

char* reglist[16]={
    "r8w",
    "r9w",
    "r10w",
    "r11w",
    "r12w",
    "r13w",
    "r14w",
    "r15w",
    "ax",
    "cx",
    "dx",
    "bx",
    "sp",
    "bp",
    "si",
    "di"
};

int regavail[16] = {0};


int getRegister()
{
    for(int i = 0; i < 16; i++){
        if(regavail[i]==0)
        {
            regavail[i]=1;
            return i;            
        }
    }
    printf("Ran out of registers\n");
    return -1;    
}

void releaseRegisters()
{
    for(int i = 0; i< 16; i++)
        regavail[i] = 0;
}

void varDeclarations(IndTable table, FILE* fp, int table_size)
{
    for(int i = 0; i < table_size; i++){
        if(table[i].isvalid == 0){
            continue;
        }
        indEntry * temp = table + i;
        while(temp != NULL) {
            // printf("%-10s%-10d%-10s%-10d\n", temp->name, temp->vtype, ftable->name, temp->offset);
            TypeList tempList = temp->tlist;
            if(tempList->next == NULL)
            {
                fprintf(fp, "\t%s resw %d\n", temp->name, 1);
            }
            else
            {
                while(tempList!=NULL)
                {
                    fprintf(fp, "\t%s.%s resw %d\n", temp->name, tempList->fieldName, 1);
                    tempList = tempList->next;
                }                
            }
            temp = temp->next;
        }
    }
    return;
}


void assignStmts(AstNode root, FILE* fp)
{
    AstNode temp = root->child;
    while(temp!=NULL)
    {
        assignStmts(temp, fp);
        temp=temp->right;
    }

    int reg1,reg2,rec1=0,rec2=0;
    char operator[OPERATOR_LENGTH];
    TypeList temprec, temprecloop1, temprecloop2;
    int fieldreg, i;
    //int traverseRegLocs1[16], traverseRegLocs2[16];

    switch(root->s.term_type)
    {
        case TK_PLUS:
        case TK_MINUS:
        case TK_MUL:
        case TK_DIV:
            if(root->child->regLocations[0]==-1)
            {
                temp = root ->child;
                if(temp->child==NULL)
                {
                    if(temp -> tlist -> next == NULL)
                    {
                        reg1 = getRegister();
                        fprintf(fp,"\tmov %s, %s\n", reglist[reg1], temp-> value);
                    }
                    else
                    {
                        rec1 = 1;
                        temprec = temp->tlist;
                        i = 0;
                        while(temprec!=NULL)
                        {
                            fieldreg = getRegister();
                            fprintf(fp,"\tmov %s, %s.%s\n", reglist[fieldreg], temp-> value, temprec->fieldName);                            
                            temp->regLocations[i] = fieldreg;
                            temprec = temprec->next;    
                            i++;                                                    
                        }
                    }
                }
                else
                {
                    reg1 = getRegister();
                    fprintf(fp, "\tmov %s, %s.%s\n", reglist[reg1], temp->value, temp->child->value);
                }
                
            }
            else
            {
                if(root->child->regLocations[1]==-1)
                    reg1 = root->child->regLocations[0];

                else
                    rec1 = 1;
            }

            if(root->child->right->regLocations[0]==-1)
            {
                temp = root->child->right;
                if(temp->child==NULL)
                {
                    if(temp -> tlist -> next == NULL)
                    {
                        reg2 = getRegister();
                        fprintf(fp, "\tmov %s, %s\n", reglist[reg2], temp-> value);
                    }
                    else
                    {
                        rec2 = 1;
                        temprec = temp->tlist;
                        i = 0;
                        while(temprec!=NULL)
                        {
                            fieldreg = getRegister();
                            fprintf(fp,"\tmov %s, %s.%s\n", reglist[fieldreg], temp-> value, temprec->fieldName);                            
                            temp->regLocations[i] = fieldreg;
                            temprec = temprec->next;
                            i++;                                                       
                        }
                    }           
                }
                else
                {
                    reg2 = getRegister();
                    fprintf(fp,"\tmov %s, %s.%s\n", reglist[reg2], temp->value,temp->child->value);
                }
            }
            else
            {
                if(root->child->right->regLocations[1]==-1)
                    reg2 = root->child->right->regLocations[0];

                else
                    rec2 = 1;
            }
            
            switch (root->s.nonterm_type)
            {
                case TK_PLUS:
                    strncpy(operator, "ADD", OPERATOR_LENGTH);
                    break;
                
                case TK_MINUS:
                    strncpy(operator, "SUB", OPERATOR_LENGTH);
                    break;

                case TK_MUL:
                    strncpy(operator, "MUL", OPERATOR_LENGTH);
                    break;

                case TK_DIV:
                    strncpy(operator, "IDIV", OPERATOR_LENGTH);
                
                
                default:
                    break;
            }
            
            if(rec1 == 0 && rec2 == 0)
            {
                fprintf(fp, "\t%s %s, %s\n", operator, reglist[reg1], reglist[reg2]);
                regavail[reg2] = 0;
                root->regLocations[0] = reg1;
                break;
            }

            int i;
            if(rec1 == 1 && rec2 == 1){
                i = 0;
                while(root->child->regLocations[i] != -1)
                {
                    fprintf(fp,"\t%s %s,%s\n", operator, reglist[root->child->regLocations[i]], reglist[root->child->right->regLocations[i]]);
                    root->regLocations[i] = root->child->regLocations[i];
                    regavail[root->child->right->regLocations[i]] = 0;
                    i++;
                }
                break;
            }

            if(rec1 == 0)
            {
                i = 0;
                while(root->child->right->regLocations[i] != -1)
                {
                    fprintf(fp, "\t%s %s,%s\n", operator, reglist[root->child->right->regLocations[i]], reglist[reg1]);
                    root->regLocations[i] = root->child->right->regLocations[i];
                    i++;
                }
                regavail[reg1] = 0;
                break;
            }
            else
            {
                i = 0;
                while(root->child->regLocations[i] != -1)
                {
                    fprintf(fp, "\t%s %s,%s\n", operator, reglist[root->child->regLocations[i]], reglist[reg2]);
                    root->regLocations[i] = root->child->regLocations[i];
                    i++;
                }
                regavail[reg2] = 0;
                break;
            }
            break;
    }


}

void condStmts(AstNode root, FILE* fp)
{
    if(root->child == NULL) return;
    AstNode temp = root-> child;
    int newreg;
    while(temp!=NULL)
    {
        condStmts(temp, fp);
        temp = temp->right;
    }


    if(root->child->regLocations[0]==-1)
    {
        newreg=getRegister();
        fprintf(fp,"\tmov %s, %s\n",reglist[newreg], root->child->value);
        root->child->regLocations[0]=newreg;
    }

    if(root->child->right !=NULL && root->child->right->regLocations[0]==-1)
    {
        newreg=getRegister();
        fprintf(fp,"\tmov %s, %s\n",reglist[newreg], root->child->right->value);
        root->child->right->regLocations[0]=newreg;
    }

    switch(root->s.term_type){
        //here
        case TK_AND:
            fprintf(fp, "\tAND %s, %s\n", reglist[root->child->regLocations[0]],reglist[root->child->right->regLocations[0]]);
            regavail[root->child->right->regLocations[0]] = 0;
            root->regLocations[0] = root->child->regLocations[0];           
            break;                                 
    
        case TK_OR:
            fprintf(fp, "\tOR %s, %s\n", reglist[root->child->regLocations[0]], reglist[root->child->right->regLocations[0]]);
            regavail[root->child->right->regLocations[0]] = 0;
            root->regLocations[0] = root->child->regLocations[0]; 
            break;           
    
        case TK_NOT:
            fprintf(fp,"\tXOR %s, 01\n",reglist[root->child->regLocations[0]]);
            root->regLocations[0] = root->child->regLocations[0];          
            break;

        case TK_GT:
            fprintf(fp,"\tCMP %s, %s\n",reglist[root->child->regLocations[0]], reglist[root->child->right->regLocations[0]]);
            fprintf(fp,"\tJG label%d\n",jumpid);
            fprintf(fp,"\tMOV %s, 0\n",reglist[root->child->regLocations[0]]);
            fprintf(fp,"\tJMP label%d\n",jumpid + 1);
            fprintf(fp,"label%d:\n\tMOV %s, 1\n", jumpid, reglist[root->child->regLocations[0]]);
            fprintf(fp,"label%d:\n", jumpid + 1);
            jumpid+=2;
            regavail[root->child->right->regLocations[0]] = 0;
            root->regLocations[0] = root->child->regLocations[0];
            break;
            
        case TK_GE:
            fprintf(fp, "\tCMP %s, %s\n",reglist[root->child->regLocations[0]], reglist[root->child->right->regLocations[0]]);
            fprintf(fp, "\tJGE label%d\n",jumpid);
            fprintf(fp, "\tMOV %s, 0\n",reglist[root->child->regLocations[0]]);
            fprintf(fp, "\tJMP label%d\n",jumpid + 1);
            fprintf(fp, "label%d:\n\tMOV %s, 1\n", jumpid, reglist[root->child->regLocations[0]]);
            fprintf(fp, "label%d:\n", jumpid + 1);
            jumpid+=2;
            regavail[root->child->right->regLocations[0]] = 0;
            root->regLocations[0] = root->child->regLocations[0];
            break;
            
        case TK_LT:
            fprintf(fp, "\tCMP %s, %s\n",reglist[root->child->regLocations[0]], reglist[root->child->right->regLocations[0]]);
            fprintf(fp, "\tJL label%d\n",jumpid);
            fprintf(fp, "\tMOV %s, 0\n",reglist[root->child->regLocations[0]]);
            fprintf(fp, "\tJMP label%d\n",jumpid + 1);
            fprintf(fp, "label%d:\n\tMOV %s, 1\n", jumpid, reglist[root->child->regLocations[0]]);
            fprintf(fp, "label%d:\n", jumpid + 1);
            jumpid+=2;
            regavail[root->child->right->regLocations[0]] = 0;
            root->regLocations[0] = root->child->regLocations[0];
            break;

        case TK_LE:
            fprintf(fp,"\tCMP %s, %s\n",reglist[root->child->regLocations[0]], reglist[root->child->right->regLocations[0]]);
            fprintf(fp,"\tJLE label%d\n",jumpid);
            fprintf(fp,"\tMOV %s, 0\n",reglist[root->child->regLocations[0]]);
            fprintf(fp,"\tJMP label%d\n",jumpid + 1);
            fprintf(fp,"label%d:\n\tMOV %s, 1\n", jumpid, reglist[root->child->regLocations[0]]);
            fprintf(fp,"label%d:\n", jumpid + 1);
            jumpid+=2;
            regavail[root->child->right->regLocations[0]] = 0;
            root->regLocations[0] = root->child->regLocations[0];
            break;

        case TK_EQ:
            fprintf(fp,"\tCMP %s, %s\n",reglist[root->child->regLocations[0]], reglist[root->child->right->regLocations[0]]);
            fprintf(fp,"\tJE label%d\n",jumpid);
            fprintf(fp,"\tMOV %s, 0\n",reglist[root->child->regLocations[0]]);
            fprintf(fp,"\tJMP label%d\n",jumpid + 1);
            fprintf(fp,"label%d:\n\tMOV %s, 1\n", jumpid, reglist[root->child->regLocations[0]]);
            fprintf(fp,"label%d:\n", jumpid + 1);
            jumpid+=2;
            regavail[root->child->right->regLocations[0]] = 0;
            root->regLocations[0] = root->child->regLocations[0];
            break;
            
        case TK_NE:
            fprintf(fp, "\tCMP %s, %s\n",reglist[root->child->regLocations[0]], reglist[root->child->right->regLocations[0]]);
            fprintf(fp, "\tJNE label%d\n",jumpid);
            fprintf(fp, "\tMOV %s, 0\n",reglist[root->child->regLocations[0]]);
            fprintf(fp, "\tJMP label%d\n",jumpid + 1);
            fprintf(fp, "label%d:\n\tMOV %s, 1\n", jumpid, reglist[root->child->regLocations[0]]);
            fprintf(fp, "label%d:\n", jumpid + 1);
            jumpid+=2;
            regavail[root->child->right->regLocations[0]] = 0;
            root->regLocations[0] = root->child->regLocations[0];
            break;
    }     
}

void codeGenHelp(AstNode root, FILE* fp)
{
    //root - main - typedefinitions - declarations - otherstmts- stmtslist
    AstNode temp = root;
    AstNode tempRhs, tempLhs;
    TypeList tempLhsTlist, tempRhsTlist;
    int newReg;
    int i = 0;
    while(temp!=NULL)
    {   
        if(temp -> t == TERM){
            if(temp->s.term_type == TK_ASSIGNOP)
            {    
                assignStmts(temp, fp);
                tempLhs = temp->child;
                tempRhs = temp->child->right;
                i = 0;
                if(tempRhs->regLocations[0] == -1){
                    if(tempRhs->child == NULL){
                        if(tempRhs->tlist->next != NULL){
                            // newReg = getRegister();
                            i = 0;
                            tempRhsTlist = tempRhs->tlist;
                            while(tempRhsTlist != NULL){
                                tempRhs->regLocations[i] = getRegister();
                                fprintf(fp, "\tMOV %s, %s.%s\n", reglist[tempRhs->regLocations[i]], tempRhs->value, tempRhsTlist->fieldName);
                                tempRhsTlist = tempRhsTlist->next;
                                i++;
                            }
                        }
                        else
                        {
                            newReg = getRegister();
                            fprintf(fp, "\tMOV %s, %s\n", reglist[newReg], tempRhs->value);
                            tempRhs->regLocations[0] = newReg;
                        }
                        
                    }
                    else
                    {
                        newReg = getRegister();
                        fprintf(fp, "\tMOV %s, %s.%s\n",reglist[newReg], tempRhs->value, tempRhs->child->value);
                        tempRhs->regLocations[0] = newReg;
                    }
                }
                if(tempLhs->child == NULL){
                    if(tempLhs->tlist->next == NULL){
                        fprintf(fp, "\tmov [%s], %s\n", tempLhs->value, reglist[tempRhs->regLocations[0]]);
                        regavail[tempRhs->regLocations[0]] = 0;
                    }
                    else{
                        tempLhsTlist = tempLhs->tlist;
                        while(tempLhsTlist != NULL && tempRhs->regLocations[i] != -1){
                            fprintf(fp, "\tmov [%s.%s], %s\n", tempLhs->value, tempLhsTlist->fieldName, reglist[tempRhs->regLocations[i]]);
                            tempLhsTlist = tempLhsTlist->next;
                            regavail[tempRhs->regLocations[i]] = 0;
                            i++;
                        }
                    }                    
                }
                else
                {
                    fprintf(fp, "\tmov [%s.%s], %s\n", tempLhs->value, tempLhs->child->value, reglist[tempRhs->regLocations[0]]);
                    regavail[tempRhs->regLocations[0]] = 0;
                }
                releaseRegisters();
            }

            else if(temp->s.term_type == TK_READ)
            {
            
            }
            else if(temp->s.term_type == TK_WRITE)
            {

            }
        }
        else
        {
            if(temp->s.nonterm_type == iterativeStmt)
            {
                //iterative - boolean expr ->right stmts
                fprintf(fp,"label%d:\n", jumpid+2);
                condStmts(temp->child, fp);
                fprintf(fp,"\tCMP %s, 0\n",reglist[temp->child->regLocations[0]]);
                fprintf(fp,"\tJE label%d\n",jumpid+1);
                codeGenHelp(temp->child->right, fp);
                fprintf(fp, "\tJMP label%d\n",jumpid);
                fprintf(fp, "label%d:\n",jumpid+1);
                jumpid+=2;
                //fprintf(fp,"\tJMP label%d\n",jumpid + 1);
                //fprintf(fp,"label%d: MOV %s, 1\n", jumpid, reglist[root->child->regLocations[0]]);
                regavail[temp->child->regLocations[0]] = 0;
                //codeGenHelp(temp->child->right->child, fp);
                //fprintf(fp,"\tJMP label%d\n",jumpid + 2);
                           
            }
            else if(temp->s.nonterm_type == conditionalStmt)
            {
                condStmts(temp->child, fp);
                fprintf(fp,"\tCMP %s, 0\n",reglist[temp->child->regLocations[0]]);
                fprintf(fp,"\tJE label%d\n",jumpid);
                fprintf(fp,"\tJMP label%d\n",jumpid + 1);
                fprintf(fp,"label%d:\n\tMOV %s, 1\n", jumpid, reglist[temp->child->regLocations[0]]);
                regavail[temp->child->regLocations[0]] = 0;
                codeGenHelp(temp->child->right->child, fp);
                fprintf(fp,"\tJMP label%d\n",jumpid + 2);

                fprintf(fp,"label%d:\n", jumpid + 1);
                regavail[temp->child->regLocations[0]] = 0;
                codeGenHelp(temp->child->right->right->child, fp);
                fprintf(fp,"label%d:\n", jumpid + 2);

                jumpid+=3;
                releaseRegisters();
                //regavail[root->child->right->regLocations[0]] = 0;
                //root->regLocations[0] = root->child->regLocations[0];
            }
        }
        
        temp = temp -> right;
    }
}

void codeGenerator(AstNode root, IndTable gtable, FuncTable ftable, char *filename)
{
    FILE* fp = fopen(filename, "w");

    //fprintf(fp, "%s\n","section .data \n \t")

    fprintf(fp, "%s", "\tglobal main\n");
    fprintf(fp, "%s", "main:\n");
    fprintf(fp, "%s", "\tsection .bss\n");

    varDeclarations(gtable, fp, GLOBAL_TABLE_SIZE);

    int mainindex = hashFunc("_main");

    funcEntry* temp = ftable + mainindex;

    while(strcmp(temp->name, "_main")!=0)
        temp = temp->next;

    varDeclarations(temp->idtable, fp, IND_TABLE_SIZE);
    fprintf(fp,"%s", "\n\tsection .text\n");
    //program - otherfunctions - mainfunction - stmts  - typedefs - declarations - stmts - stmt list
    codeGenHelp(root->child->right->child->child->right->right->child, fp);
    fprintf(fp, "\tmov eax, 0\n\tret\n");
    fclose(fp);
}