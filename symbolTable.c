#include "symbolDef.h"
#include "symbolTable.h"

extern int errflag;

#define INT_WIDTH 2
#define REAL_WIDTH 4

funcEntry ftable[F_TABLE_SIZE];
recEntry rectable[REC_TABLE_SIZE];
int globaloffset = 0;
globalEntry gtable[GLOBAL_TABLE_SIZE];

char *map_vtype_to_strings[4] = {
    "int", "real", "record", "error"
};

int hashFunc(char* key)
{
	unsigned long hash = 5381;
	int c;
	while (c = *key++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % F_TABLE_SIZE);
}

int hashRec(char* key)
{
	unsigned long hash = 5381;
	int c;
	while (c = *key++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % REC_TABLE_SIZE);
}

int hashInd(char* key)
{
	unsigned long hash = 5381;
	int c;
	while (c = *key++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % IND_TABLE_SIZE);
}

int hashGlob(char* key)
{
	unsigned long hash = 5381;
	int c;
	while (c = *key++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % GLOBAL_TABLE_SIZE);
}

IndTable initializeIndTable(){
    IndTable indtable = (IndTable) malloc(sizeof(indEntry) * IND_TABLE_SIZE);
    for(int i = 0; i < IND_TABLE_SIZE; i++){
        indtable[i].isvalid = 0;
        indtable[i].width = 0;
        indtable[i].offset = 0;
        indtable[i].next = NULL;
        indtable[i].tlist = NULL;
        indtable[i].name[0] = '\0';
    }
    return indtable;
}

void initializeRecTable(){
    //RecTable rectable = (RecTable) malloc(sizeof(indEntry) * REC_TABLE_SIZE);
    for(int i = 0; i < REC_TABLE_SIZE; i++){
        rectable[i].isvalid = 0;
        rectable[i].width = 0;
        rectable[i].tlist=NULL;
        rectable[i].next = NULL;
        rectable[i].name[0] = '\0';
    }
}

void initializeGlobalTable(){
    //RecTable rectable = (RecTable) malloc(sizeof(indEntry) * REC_TABLE_SIZE);
    for(int i = 0; i < GLOBAL_TABLE_SIZE; i++){
        gtable[i].isvalid = 0;
        gtable[i].width = 0;
        gtable[i].tlist=NULL;
        gtable[i].next = NULL;
        gtable[i].name[0] = '\0';
    }
}

void initializeFuncTable(){
    //FuncTable ftable = (FuncTable) malloc(sizeof(funcEntry) * F_TABLE_SIZE);
    for(int i = 0; i < F_TABLE_SIZE; i++){
        ftable[i].isvalid = 0;
        ftable[i].input_par = NULL;
        ftable[i].output_par = NULL;
        ftable[i].next = NULL;
        ftable[i].idtable = NULL;
        ftable[i].name[0] = '\0';
    }
}

void addVariable(char *funcName, AstNode varNode, int offs, int wid)
{
    int findex = hashFunc(funcName);
    funcEntry* temp = ftable + findex;  //relevant entry in the function table
    /* while(strcmp(funcName, temp->name) != 0){
        temp = temp->next;
    }
    if(temp->idtable==NULL)
    {
        temp->idtable = initializeIndTable();
    } */
    while(strcmp(temp->name, funcName) != 0){
        temp = temp->next;
    }
    int findvar = hashInd(varNode->value);
    indEntry *tempind = temp->idtable + findvar;
    if(tempind->isvalid == 0)
    {
        tempind->isvalid = 1;
        // tempind->vtype = varNode->tlist->vtype;
        
        //tempind->tlist = (TypeList) malloc(sizeof(struct typeList));
        tempind->tlist = varNode->tlist;
        //tempind->tlist->next = NULL;
        
        strncpy(tempind->name, varNode->value, MAX_LEXEME_SIZE+1);
        tempind->width = wid;
        tempind->offset = offs;
    }
    else
    {
        while(tempind->next!=NULL && strcmp(tempind->name,varNode->value)!=0)
        {
            tempind = tempind->next;
        }
        if(tempind->next!=NULL)
        {
            printf("Line %d: Redeclaration of variable %s\n",varNode->line_number, varNode->value);
            errflag=1;
        }
        else
        {
            tempind->next = (struct indEntry*)malloc(sizeof(indEntry));
            tempind->next->isvalid=1;
            // tempind->next->vtype = varNode->tlist->vtype;
            
            //tempind->next->tlist = (TypeList) malloc(sizeof(struct typeList));
            tempind->next->tlist = varNode->tlist;
            //tempind->next->tlist->next = NULL;
            
            strncpy(tempind->next->name, varNode->value, MAX_LEXEME_SIZE+1);
            tempind->next->width = wid;
            tempind->next->offset = offs;                        
        }
        
    }
    return;
}

void addGlobalVariable(AstNode varNode){
    int glbIndex = hashGlob(varNode->value);
    indEntry* glbEntry = gtable + glbIndex;
    if(glbEntry->isvalid == 0){  //no entry made yet
        
        //return;
    }

    else{
        while(glbEntry->next != NULL){   //traverse chain
            if(strcmp(glbEntry->name, varNode->value) == 0){
                printf("Line %d: Redeclaration of global variable %s\n", varNode->line_number, varNode->value);
                errflag = 1;
                return;
            }
            glbEntry = glbEntry->next;
        }
        if(strcmp(glbEntry->name, varNode->value) == 0){
                printf("Line %d: Redeclaration of global variable %s\n", varNode->line_number, varNode->value);
                errflag = 1;
                return;
        }
        glbEntry->next = (indEntry*) malloc(sizeof(indEntry));
        glbEntry=glbEntry->next;
    }
    glbEntry->isvalid = 1;
    glbEntry->next = NULL;
    glbEntry->offset = globaloffset;
    strncpy(glbEntry->name, varNode->value, MAX_LEXEME_SIZE+1);
    recEntry* temprec; 

    switch (varNode->tlist->vtype)
    {
        case intType:
            globaloffset += INT_WIDTH;
            break;

        case realType:
            globaloffset += REAL_WIDTH;
            break;

        case recordType:                    
            temprec = recordTableLookup(varNode->recName);  //record table lookup
            if(temprec == NULL){
                printf("Line %d: Undefined record of type %s\n", varNode->line_number, varNode->recName);
                errflag = 1;
            }
            varNode -> tlist = temprec->tlist;

            globaloffset += temprec->width;
            break;
    
        default:
            break;
    }
    glbEntry->tlist = varNode->tlist;
    return;
}

void addFunction(AstNode funcNode)
{
    int index = hashFunc(funcNode -> value);
    int offset = 0, wid;
    AstNode fieldnode;
    AstNode tnode;
    recEntry *temprec;
    if(ftable[index].isvalid == 0){
        strncpy(ftable[index].name, funcNode->value, sizeof(funcNode->value));
        ftable[index].idtable = initializeIndTable();

        //first add input parameters to this function's symbol table
        if(funcNode->s.term_type != TK_MAIN){
            tnode = funcNode->child->child;
            while(tnode!=NULL){
                if(symbolTableLookup(funcNode->value, tnode->value)!=NULL){
                    printf("Line %d: Redeclaration of variable %s\n",tnode->line_number, tnode->value);
                    errflag = 1;
                    tnode = tnode -> right;
                    continue;
                }
                switch(tnode->tlist->vtype){
                    case intType:
                        addVariable(funcNode->value, tnode, offset, INT_WIDTH);
                        offset += INT_WIDTH;
                        break;
                    case realType:
                        addVariable(funcNode->value, tnode, offset, REAL_WIDTH);
                        offset += REAL_WIDTH;
                        break;
                    case recordType:                    
                        temprec = recordTableLookup(tnode->recName);  //record table lookup
                        if(temprec == NULL){
                            printf("Line %d: Undefined record of type %s\n", tnode->line_number, tnode->recName);
                            errflag = 1;
                            break;
                        }
                        wid = temprec->width;
                        tnode -> tlist = temprec->tlist;
                        addVariable(funcNode->value, tnode, offset, wid);
                        offset += wid;
                        break;
                }
            tnode = tnode->right;
            }
        }

        //now output parameters
        if(funcNode->s.term_type != TK_MAIN){
            tnode = funcNode->child->right->child;
            while(tnode!=NULL){
                if(symbolTableLookup(funcNode->value, tnode->value)!=NULL){
                    printf("Line %d: Redeclaration of variable %s\n",tnode->line_number, tnode->value);
                    errflag = 1;
                    tnode = tnode -> right;
                    continue;
                }
                switch(tnode->tlist->vtype){
                    case intType:
                        addVariable(funcNode->value, tnode, offset, INT_WIDTH);
                        offset += INT_WIDTH;
                        break;
                    case realType:
                        addVariable(funcNode->value, tnode, offset, REAL_WIDTH);
                        offset += REAL_WIDTH;
                        break;
                    case recordType:                    
                        temprec = recordTableLookup(tnode->recName);  //record table lookup
                        if(temprec == NULL){
                            printf("Line %d: Undefined record of type %s\n", tnode->line_number, tnode->recName);
                            errflag = 1;
                            break;
                        }
                        wid = temprec->width;
                        tnode -> tlist = temprec->tlist;
                        addVariable(funcNode->value, tnode, offset, wid);
                        offset += wid;
                        break;
                }
            tnode = tnode->right;
            }
        }
        
        //now to declarations
        // AstNode tnode = funcNode->child->right->right->child->right->child;
        //funid - inputpar - output par - stmts - typedefinitions - declarations - individual declaration
        if(funcNode->s.term_type != TK_MAIN)
        {
            tnode = funcNode->child->right->right->child->right->child;
        }
        //tk_main - stmts - typedefinitions - individual definition (has recordid)  children are fieldids
        else
        {
            tnode = funcNode->child->child->right->child;
        }
        while(tnode!=NULL){
            if(tnode->globalFlg){
                // printf("Adding global variable %s\n", tnode->value);
                //addGlobalVariable(tnode);
                tnode = tnode->right;
                //global variables have already been added
                continue;
            }
            if(symbolTableLookup(funcNode->value, tnode->value)!=NULL)
            {
                printf("Line %d: Redeclaration of variable %s\n",tnode->line_number, tnode->value);
                errflag = 1;
                tnode = tnode -> right;
                continue;
            }
            switch(tnode->tlist->vtype){
                case intType:
                    addVariable(funcNode->value, tnode, offset, INT_WIDTH);
                    offset += INT_WIDTH;
                    break;
                case realType:
                    addVariable(funcNode->value, tnode, offset, REAL_WIDTH);
                    offset += REAL_WIDTH;
                    break;
                case recordType:                    
                    temprec = recordTableLookup(tnode->recName);  //record table lookup
                    if(temprec == NULL){
                        printf("Line %d: Undefined record of type %s\n", tnode->line_number, tnode->recName);
                        errflag = 1;
                        break;
                    }
                    wid = temprec->width;
                    tnode -> tlist = temprec->tlist;
                    addVariable(funcNode->value, tnode, offset, wid);
                    offset += wid;
                    break;
            }
            tnode = tnode->right;
        }
        if(funcNode->s.term_type != TK_MAIN){
            ftable[index].input_par = funcNode->child->child;
            ftable[index].output_par = funcNode->child->right->child;
        }
        ftable[index].next = NULL;
        ftable[index].isvalid = 1;
    }
    else{
        FuncTable temp = (struct funcEntry*) &ftable[index];
        while(temp->next != NULL){
            if(strcmp(funcNode->value, temp->name) == 0){
                printf("Line %d: Redeclaration of function %s\n", funcNode->line_number, funcNode->value);
                errflag = 1;
                return;
            }
            temp = temp->next;
        }
        struct funcEntry* f;
        f = (struct funcEntry*) malloc(sizeof(struct funcEntry));
        strncpy(f->name, funcNode->value, MAX_LEXEME_SIZE + 1);
        f->isvalid = 1;
        f->idtable = initializeIndTable();
        temp->next = f;


        //first add input parameters to this function's symbol table
        if(funcNode->s.term_type != TK_MAIN){
            tnode = funcNode->child->child;
            while(tnode!=NULL){
                if(symbolTableLookup(funcNode->value, tnode->value)!=NULL){
                    printf("Line %d: Redeclaration of variable %s\n",tnode->line_number, tnode->value);
                    errflag = 1;
                    tnode = tnode -> right;
                    continue;
                }
                switch(tnode->tlist->vtype){
                    case intType:
                        addVariable(funcNode->value, tnode, offset, INT_WIDTH);
                        offset += INT_WIDTH;
                        break;
                    case realType:
                        addVariable(funcNode->value, tnode, offset, REAL_WIDTH);
                        offset += REAL_WIDTH;
                        break;
                    case recordType:                    
                        temprec = recordTableLookup(tnode->recName);  //record table lookup
                        if(temprec == NULL){
                            printf("Line %d: Undefined record of type %s\n", tnode->line_number, tnode->recName);
                            errflag = 1;
                            break;
                        }
                        wid = temprec->width;
                        tnode -> tlist = temprec->tlist;
                        addVariable(funcNode->value, tnode, offset, wid);
                        offset += wid;
                        break;
                }
            tnode = tnode->right;
            }
        }

        //now output parameters
        if(funcNode->s.term_type != TK_MAIN){
            tnode = funcNode->child->right->child;
            while(tnode!=NULL){
                if(symbolTableLookup(funcNode->value, tnode->value)!=NULL){
                    printf("Line %d: Redeclaration of variable %s\n",tnode->line_number, tnode->value);
                    errflag = 1;
                    tnode = tnode -> right;
                    continue;
                }
                switch(tnode->tlist->vtype){
                    case intType:
                        addVariable(funcNode->value, tnode, offset, INT_WIDTH);
                        offset += INT_WIDTH;
                        break;
                    case realType:
                        addVariable(funcNode->value, tnode, offset, REAL_WIDTH);
                        offset += REAL_WIDTH;
                        break;
                    case recordType:                    
                        temprec = recordTableLookup(tnode->recName);  //record table lookup
                        if(temprec == NULL){
                            printf("Line %d: Undefined record of type %s\n", tnode->line_number, tnode->recName);
                            errflag = 1;
                            break;
                        }
                        wid = temprec->width;
                        tnode -> tlist = temprec->tlist;
                        addVariable(funcNode->value, tnode, offset, wid);
                        offset += wid;
                        break;
                }
            tnode = tnode->right;
            }
        }
        
        //now to declarations
        // AstNode tnode = funcNode->child->right->right->child->right->child;
        //funid - inputpar - output par - stmts - typedefinitions - declarations - individual declaration
        if(funcNode->s.term_type != TK_MAIN)
        {
            tnode = funcNode->child->right->right->child->right->child;
        }
        //tk_main - stmts - typedefinitions - individual definition (has recordid)  children are fieldids
        else
        {
            tnode = funcNode->child->child->right->child;
        }
        while(tnode!=NULL){
            if(tnode->globalFlg){
                // printf("Adding global variable %s\n", tnode->value);
                //addGlobalVariable(tnode);
                tnode = tnode->right;
                //global variables have already been added
                continue;
            }
            if(symbolTableLookup(funcNode->value, tnode->value)!=NULL)
            {
                printf("Line %d: Redeclaration of variable %s\n",tnode->line_number, tnode->value);
                errflag = 1;
                tnode = tnode -> right;
                continue;
            }
            switch(tnode->tlist->vtype){
                case intType:
                    addVariable(funcNode->value, tnode, offset, INT_WIDTH);
                    offset += INT_WIDTH;
                    break;
                case realType:
                    addVariable(funcNode->value, tnode, offset, REAL_WIDTH);
                    offset += REAL_WIDTH;
                    break;
                case recordType:                    
                    temprec = recordTableLookup(tnode->recName);  //record table lookup
                    if(temprec == NULL){
                        printf("Line %d: Undefined record of type %s\n", tnode->line_number, tnode->recName);
                        errflag = 1;
                        break;
                    }
                    wid = temprec->width;
                    tnode -> tlist = temprec->tlist;
                    addVariable(funcNode->value, tnode, offset, wid);
                    offset += wid;
                    break;
            }
            tnode = tnode->right;
        }
        if(funcNode->s.term_type != TK_MAIN){
            f->input_par = funcNode->child->child;
            f->output_par = funcNode->child->right->child;
        }
        f->next = NULL;
        f->isvalid = 1;
    }
}

void printIdTable(indEntry *idtable, funcEntry *ftable){
    printf("LEXEME%-10stype%-10sscope%-10soffset\n", " ", " ", " ");
    for(int i = 0; i < IND_TABLE_SIZE; i++){
        if(idtable[i].isvalid == 0){
            continue;
        }
        indEntry * temp = idtable + i;
        while(temp != NULL) {
            // printf("%-10s%-10d%-10s%-10d\n", temp->name, temp->vtype, ftable->name, temp->offset);
            printf("%-15s", temp->name);
            TypeList tempList = temp->tlist;
            printf("%s", map_vtype_to_strings[tempList->vtype]);
            tempList = tempList->next;
            while(tempList != NULL){
                printf(" x %s", map_vtype_to_strings[tempList->vtype]);
                tempList = tempList->next;
            }
            printf("%20s", " ");
            printf("%-15s%-15d\n", ftable->name, temp->offset);
            temp = temp->next;
        }
    }
}

void printGlobalTable(){
    // printf("Printing global table\n\n");
    printf("LEXEME%-20stype%-20sscope%-20soffset\n", " ", " ", " ");
    for(int i = 0; i < GLOBAL_TABLE_SIZE; i++){
        if(gtable[i].isvalid == 0){
            continue;
        }
        indEntry * temp = gtable + i;
        while(temp != NULL) {
            // printf("%-10s%-10d%-10s%-10d\n", temp->name, temp->vtype, ftable->name, temp->offset);
            printf("%-20s", temp->name);
            TypeList tempList = temp->tlist;
            printf("%s", map_vtype_to_strings[tempList->vtype]);
            tempList = tempList->next;
            while(tempList != NULL){
                printf(" x %s", map_vtype_to_strings[tempList->vtype]);
                tempList = tempList->next;
            }
            printf("%20s", " ");
            printf("%10s%20d\n", "global", temp->offset);
            temp = temp->next;
        }
    }
}

void printRecordTable(){
    printf("Record Name%-20stype%-20swidth\n", " ", " ");
    for(int i = 0; i < REC_TABLE_SIZE; i++){
        if(rectable[i].isvalid == 0){
            continue;
        }
        recEntry *tempRec = rectable + i;
        while(tempRec != NULL){
            printf("%-20s", tempRec->name);
            TypeList tempList = tempRec->tlist;
            printf("%s", map_vtype_to_strings[tempList->vtype]);
            tempList = tempList->next;
            while(tempList != NULL){
                printf(" x %s", map_vtype_to_strings[tempList->vtype]);
                tempList = tempList->next;
            }
            printf("%20s", " ");
            printf("%10s%20d\n", "global", tempRec->width);
            tempRec = tempRec->next;
        }
    }
}

void printFunctionTable(){
    for(int i = 0; i < F_TABLE_SIZE; i++){
        if(ftable[i].isvalid == 0){
            continue;
        }
        funcEntry * temp = &ftable[i];
        while(temp != NULL){
            printf("FUNID\t\tinput_par\t\toutput_par\n%-15s", temp->name);
            printIdTable(temp->idtable, temp);
            temp = temp->next;
        }
    }
}

void addRecords(AstNode funcNode)
{
    AstNode temp, child1, child2;
    //funid - inputpar - output par - stmts - typedefinitions - individual definition (has recordid)  children are fieldids
    if(funcNode->s.term_type != TK_MAIN)
    {
        temp = funcNode->child->right->right->child->child;
    }
    
    //tk_main - stmts - typedefinitions - individual definition (has recordid)  children are fieldids
    else
    {
        temp = funcNode->child->child->child;
    }

    while(temp!=NULL)
    {
        child1 = temp ->child;
        child2 = temp ->child->right;

        while(child1!=NULL)
        {
            while(child2!=NULL)
            {
                if(strcmp(child1->value,child2->value)==0)
                {
                    printf("Line %d: Two fields of a record cannot have the same name\n",child2->line_number);
                    errflag = 1;
                }
                child2=child2->right;
            }
            child1=child1->right;
        }

        int index = hashRec(temp->value);
        //recEntry* addentry = rectable + index;
        recEntry* tempentry = rectable + index;
        if(tempentry->isvalid)
        {
            while( tempentry->next!=NULL && strcmp(tempentry->name, temp->value)!=0)
            {
                tempentry = tempentry->next;
            }
            if(tempentry -> next != NULL)
            {
                printf("Line %d: Redefinition of record %s\n", temp -> line_number, temp -> value);
                errflag = 1;
                temp = temp -> right;
                break;
            }
            // recEntry r;
            tempentry->next = (recEntry *) malloc(sizeof(recEntry));
            tempentry = tempentry->next;
        }

        tempentry->isvalid = 1;
        tempentry->tlist = temp->tlist;
        strncpy(tempentry->name, temp->value, MAX_LEXEME_SIZE + 1);
        tempentry -> next = NULL;

        TypeList t = temp -> tlist; //calculate width here
        int width = 0;
        while(t != NULL){
            switch (t->vtype)
            {
                case intType:
                    width += INT_WIDTH;
                    break;

                case realType:
                    width += REAL_WIDTH;
                    break;
            
                default:
                    break;
            }
            t = t->next;
        }
        tempentry->width = width;
        temp = temp->right;
    }
    return;
}

void addFunctions(AstNode root)
{
    AstNode mainfun;
    
    if(root->child->t = NON_TERM && root->child->s.nonterm_type == otherFunctions)
    {
        AstNode temp = root->child->child;
        while(temp!=NULL)
        {
            addFunction(temp);
            temp = temp -> right;
        }
    }
    
    mainfun = root->child->right;
    //mainfunc -> stmts -> typedefinitions -> declarations
    addFunction(mainfun);
    return;
}

indEntry* symbolTableLookup(char *funcName, char *varName){
    int funcIndex = hashFunc(funcName), idIndex = hashInd(varName), glbIndex = hashGlob(varName);
    /* if(ftable[funcIndex].isvalid == 0){
        return NULL;
    } */
    funcEntry *tempFunc = ftable + funcIndex;
    indEntry *temp;
    while(tempFunc != NULL){
        if(strcmp(tempFunc->name, funcName) == 0){
            break;
        }
        tempFunc = tempFunc->next;
    }
    if(tempFunc == NULL){
        
    }
    else{
        temp = tempFunc->idtable + idIndex;
        if(temp->isvalid == 0){
        // return NULL;
        temp = NULL;
        }
        while(temp != NULL){
            if(strcmp(temp->name, varName) == 0){
                return temp;
            }
            temp = temp->next;
        }
    }
    
    //check global table if no entry found in local symboltable
    temp = gtable + glbIndex;
    if(temp->isvalid == 0){
        return NULL;
    }
    while(temp != NULL){
        if(strcmp(temp->name, varName) == 0){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

indEntry* globalTableLookup(char *varName){
    int glbIndex = hashGlob(varName);
    indEntry *temp = gtable + glbIndex;
    if(temp->isvalid == 0){
        return NULL;
    }
    while(temp != NULL){
        if(strcmp(temp->name, varName) == 0){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

recEntry* recordTableLookup(char* recName){
    int recIndex = hashRec(recName);
    recEntry* temp = rectable + recIndex;
    if(temp->isvalid == 0){
        return NULL;
    }
    while(temp != NULL){
        if(strcmp(recName, temp->name) == 0){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

funcEntry* funcTableLookup(char* funcName){
    int funcIndex = hashFunc(funcName);
    funcEntry *temp = ftable + funcIndex;
    if(temp->isvalid == 0){
        return NULL;
    }
    while(temp != NULL){
        if(strcmp(temp->name, funcName) == 0){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void addGlobalVariables(AstNode root)   //initialize global table
{
    AstNode temp = root->child->child;
    AstNode tnode;

    while(temp!=NULL)
    {     
        //funid - inputpar - output par - stmts - typedefinitions - declarations - individual declaration
        tnode = temp->child->right->right->child->right->child;
        while(tnode!=NULL)
        {
            if(tnode->globalFlg)
            {
                // printf("Adding global variable %s\n", tnode->value);
                addGlobalVariable(tnode);
                //continue;
            }
            tnode = tnode->right;
        }
        temp = temp -> right;
    }
    //ast root - otherfuncs - mainfunc - stmts - typedefinitions - declarations - individual declaration
    tnode = root->child->right->child->child->right->child;
    while(tnode!=NULL)
    {
        if(tnode->globalFlg)
        {
                // printf("Adding global variable %s\n", tnode->value);
            addGlobalVariable(tnode);
                //continue;
        }
        tnode = tnode->right;
    }
    return;
}

void addAllRecords(AstNode root)
{
    if(root->child->child != NULL)
    {
        AstNode temp = root->child->child;
        while(temp!=NULL)
        {
            addRecords(temp);
            temp = temp -> right;
        }
    }
    addRecords(root->child->right);
    return;
}

void populateSymbolTables(AstNode root){
    // initialize all tables
    initializeGlobalTable();
    initializeRecTable();
    initializeFuncTable();

    addAllRecords(root);
    addGlobalVariables(root);
    addFunctions(root);
}

void printSymbolTable(){
    //global table
    printGlobalTable();

    for(int i = 0; i < F_TABLE_SIZE; i++){
        if(!ftable[i].isvalid){
            continue;
        }
        funcEntry *tempFunc = ftable + i;
        while(tempFunc != NULL){
            IndTable idtable = tempFunc->idtable;
            for(int j = 0; j < IND_TABLE_SIZE; j++){
                if(idtable[j].isvalid == 0){
                    continue;
                }
                indEntry * temp = idtable + j;
                while(temp != NULL) {
                    // printf("%-10s%-10d%-10s%-10d\n", temp->name, temp->vtype, ftable->name, temp->offset);
                    printf("%-20s", temp->name);
                    TypeList tempList = temp->tlist;
                    printf("%s", map_vtype_to_strings[tempList->vtype]);
                    tempList = tempList->next;
                    while(tempList != NULL){
                        printf(" x %s", map_vtype_to_strings[tempList->vtype]);
                        tempList = tempList->next;
                    }
                    printf("%20s", " ");
                    printf("%10s%20d\n", tempFunc->name, temp->offset);
                    temp = temp->next;
                }
            }
            tempFunc = tempFunc->next;                        
        }
    }
}


int getIndMems(IndTable table)
{
    int totalwidth = 0;
    for(int i = 0; i < IND_TABLE_SIZE; i++){
        if(table[i].isvalid == 0){
            continue;
        }
        indEntry * temp = table + i;
        while(temp != NULL) {
            totalwidth += temp -> width;
            temp = temp->next;
        }
    }
    return totalwidth;
}

void printFuncMems()
{
    int funcwidth = 0;
    for(int i = 0; i < F_TABLE_SIZE ; i++)
    {
        if(ftable[i].isvalid == 0)
            continue;
        funcEntry *temp = ftable + i;
        while(temp != NULL)
        {
            funcwidth = getIndMems(temp->idtable);
            printf("%15s\t%10d\n",temp->name, funcwidth);
            temp = temp -> next;
        }
    }
}