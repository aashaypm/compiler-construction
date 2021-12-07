/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#include "lexerDef.h"
#include "lexer.h"
#include "parserDef.h"
#include "stackTree.h"

int size_of_first_set[TOTAL_NUM_NT];
int size_of_follow_set[TOTAL_NUM_NT];
TK_TYPES first[TOTAL_NUM_NT][20];
TK_TYPES follow[TOTAL_NUM_NT][20];
int is_visited_node[TOTAL_NUM_NT];
int ParseTable[TOTAL_NUM_NT][TOTAL_NUM_TOKENS];
int goes_to_eps[TOTAL_NUM_NT];

extern int parseTreeMem; //total bytes take by parsetree
int no_syntax_errors = 1; //global variable, indicator of syntactic correctness of source code

char *map_terminals_to_strings[TOTAL_NUM_TOKENS] = {
	"TK_ASSIGNOP",
	"TK_COMMENT",
	"TK_FIELDID",
	"TK_ID",
	"TK_NUM",
	"TK_RNUM",
	"TK_FUNID",
	"TK_RECORDID",
	"TK_WITH",
	"TK_PARAMETERS",
	"TK_END",
	"TK_WHILE",
	"TK_TYPE",
	"TK_MAIN",
	"TK_GLOBAL",
	"TK_PARAMETER",
	"TK_LIST",
	"TK_SQL",
	"TK_SQR",
	"TK_INPUT",
	"TK_OUTPUT",
	"TK_INT",
	"TK_REAL",
	"TK_COMMA",
	"TK_SEM",
	"TK_COLON",
	"TK_DOT",
	"TK_ENDWHILE",
	"TK_OP",
	"TK_CL",
	"TK_IF",
	"TK_THEN",
	"TK_ENDIF",
	"TK_READ",
	"TK_WRITE",
	"TK_RETURN",
	"TK_PLUS",
	"TK_MINUS",
	"TK_MUL",
	"TK_DIV",
	"TK_CALL",
	"TK_RECORD",
	"TK_ENDRECORD",
	"TK_ELSE",
	"TK_AND",
	"TK_OR",
	"TK_NOT",
	"TK_LT",
	"TK_LE",
	"TK_EQ",
	"TK_GT",
	"TK_GE",
	"TK_NE",
	"eps",
	"$"
};

char *map_nonterminals_to_strings[TOTAL_NUM_NT] = {
	"program",
	"mainFunction",
	"otherFunctions",
	"function",
	"input_par",
	"output_par",
	"parameter_list",
	"dataType",
	"remaining_list",
	"primitiveDatatype",
	"constructedDatatype",
	"stmts",
	"typeDefinitions",
	"typeDefinition",
	"fieldDefinitions",
	"fieldDefinition",
	"moreFields",
	"declarations",
	"declaration",
	"global_or_not",
	"otherStmts",
	"stmt",
	"assignmentStmt",
	"singleOrRecId",
	"new24",
	"funCallStmt",
	"outputParameters",
	"inputParameters",
	"iterativeStmt",
	"conditionalStmt",
	"elsePart",
	"ioStmt",
	"allVar",
	"newVar",
	"arithmeticExpression",
	"expPrime",
	"term",
	"termPrime",
	"factor",
	"highPrecedenceOperators",
	"lowPrecedenceOperators",
	"all",
	"temp",
	"booleanExpression",
	"var",
	"logicalOp",
	"relationalOp",
	"returnStmt",
	"optionalReturn",
	"idList",
	"more_ids"
};

struct grHead gram_rules[RULE_COUNT];

int find(char *str, int t_or_nt){

	int i;
	if(t_or_nt){
		for(i = 0; i < TOTAL_NUM_NT; i++){
			if(strcmp(str, map_nonterminals_to_strings[i]) == 0)
				return i;
		}
	}

	else{
		for(i = 0; i < TOTAL_NUM_TOKENS; i++){
			if(strcmp(str, map_terminals_to_strings[i]) == 0)
				return i;
		}
	}
}

void insertAtEnd(GrNode node, int rule_index){

	GrNode temp = gram_rules[rule_index].first;
	if(temp == NULL){
		gram_rules[rule_index].first = node;
		gram_rules[rule_index].last=node;
		return;
	}

	while(temp->next != NULL)
	{	
		temp = temp->next;
	}
	temp->next = node;
	node->prev=temp;
	gram_rules[rule_index].last=node;
	return;
}

void loadGrammar()
{
	FILE* fp = fopen("grammar.txt","r");
	int i,j;
	char *ch;
	char gline[100];
	char tnt[50];
	
	for(i = 0; i < RULE_COUNT; i++)
	{
		fgets(gline,100,fp);
		ch = gline;
		j = 0;
		while(*ch != ' ')
		{
			tnt[j]=*ch;
			j++;
			ch++;
		}
		tnt[j] = '\0';						// lhs of grammar rule ends here

		//make grhead
		int nt_index = find(tnt, 1);
		gram_rules[i].nonterm_head = nt_index;
		gram_rules[i].first = NULL;
		gram_rules[i].last=NULL;

		while(*ch != '>')
			ch++;
		ch++;

		while(*ch != '\0')
		{
			if(*ch >= 'a' && *ch <= 'z')
			{
				j=0;
				while(*ch != ' ' && *ch != '\n' && *ch != '\r')
				{
					tnt[j]=*ch;
					j++;
					ch++;
				}
				tnt[j]='\0';

				//handle this nonterminal
				//construct grnode
				if(strcmp(tnt, "eps") != 0)
				{
					GrNode nt_node = (GrNode) malloc(sizeof(struct grNode));
					nt_node->t = 1;
					nt_node->s.nonterm_type = find(tnt, 1);
					nt_node->next = NULL;
					nt_node->prev=NULL;
					//parseTreeMem += sizeof(nt_node);
					//insert this node in the linkedlist corresponding to ith rule
					insertAtEnd(nt_node, i);
				}
				else
				{
					GrNode t_node = (GrNode) malloc(sizeof(struct grNode));
					t_node->t = 0;
					t_node->s.term_type = find(tnt, 0);
					t_node->next = NULL;
					t_node->prev=NULL;
					//parseTreeMem += sizeof(t_node);
					//insert this node in the linkedlist corresponding to ith rule
					insertAtEnd(t_node, i);
				}
			}

			else if(*ch >= 'A' && *ch <= 'Z')
			{
				j=0;
				while(*ch != ' ' && *ch != '\n' && *ch != '\r')
				{
					tnt[j] = *ch;
					j++;
					ch++;
				}
				tnt[j] = '\0';

				//handle this terminal
				//construct grnode
				GrNode t_node = (GrNode) malloc(sizeof(struct grNode));
				t_node->t = 0;
				t_node->s.term_type = find(tnt, 0);
				t_node->next = NULL;
				t_node->prev=NULL;
				//parseTreeMem += sizeof(t_node);
				//insert this node in the linkedlist corresponding to ith rule
				insertAtEnd(t_node, i);
			}
			else
				ch++;	

		}

	}
	fclose(fp);
	return;

}


void freeGrammar()
{
	int i;
	GrNode temp;
	for(i=0;i<RULE_COUNT;i++)
	{
		temp=gram_rules[i].first;
		while(temp->next!=NULL)
		{
			temp=temp->next;
			free(temp->prev);
			temp->prev=NULL;
		}
		free(temp);
	}
}

//--------------------------------------------------------------------------------------------------------------------------


void addToFirstSet(TK_TYPES ter, int index)
{
	int i=0;
	while(i < size_of_first_set[index])
	{
		if(first[index][i] == ter)
		{
			break;
		}
		i++;
	}

	if(i == size_of_first_set[index])
	{
		first[index][size_of_first_set[index]] = ter;
		size_of_first_set[index]++;
	}
	return;
}

void addToFollowSet(TK_TYPES ter, int index)
{
	int i=0;
	while(i < size_of_follow_set[index])
	{
		if(follow[index][i] == ter)
		{
			break;
		}
		i++;
	}

	if(i == size_of_follow_set[index])
	{
		follow[index][size_of_follow_set[index]] = ter;
		size_of_follow_set[index]++;
	}
	return;
}

void search(TK_NTTYPES nonter)
{
	if(is_visited_node[nonter])
	{
		return;
	}
	int i=0, j;
	while(i < RULE_COUNT)
	{
		if(gram_rules[i].nonterm_head == nonter)
		{
			GrNode current = gram_rules[i].first;
			while(1)
			{
				if(current->t == TERM)
				{
					addToFirstSet(current->s.nonterm_type, nonter);
					if(current->s.term_type == TK_EPS){
						goes_to_eps[nonter] = 1;
					}
					break;
				}
				search(current->s.nonterm_type);
				int epsilon = 0;
				int p = current->s.nonterm_type;
				j=0;
				while(1)
				{
					if (first[p][j] == -1)
					{
						break;
					}
					if(strcmp(map_terminals_to_strings[first[p][j]],"eps")!=0)
					{
						addToFirstSet(first[p][j], nonter);
					}
					else
					{
						epsilon = 1;
					}
					j++;	
				}
				if(epsilon == 0)
				{
					break;
				}
				else
				{
					current = current->next;
					if(current == NULL)
					{
						addToFirstSet(TK_EPS,nonter);
						break;
					}
				}
			}		
		}
		i++;
	}
	is_visited_node[nonter] = 1;
}

void computeFirstSet()
{
	int i=0, j;
	for(int k = 0; k < TOTAL_NUM_NT; k++){
		goes_to_eps[k] = 0;
	}
	while(i < TOTAL_NUM_NT)
	{
		size_of_first_set[i] = 0;
		is_visited_node[i] = 0;
		j=0;
		while(j < 20)
		{
			first[i][j] = -1;
			j++;
		}
		i++;
	}

	i=0;	
	while(i < RULE_COUNT)
	{
		TK_NTTYPES nonter=gram_rules[i].nonterm_head;
		if(!is_visited_node[nonter])
		{
			search(nonter);
			is_visited_node[nonter] = 1;
		}
		i++;
	}
}

void computeFollowSet()
{
	int i=0, j, k;
	while(i < TOTAL_NUM_NT)
	{
		size_of_follow_set[i] = 0;
		j=0;
		while(j < 20)
		{
			follow[i][j] = -1;
			j++;
		}
		i++;
	}
	addToFollowSet(TK_DOL, 0);

	for(k = 0; k < 2; k++)
	{
		i=0;
		while(i < RULE_COUNT)
		{
			GrNode current = gram_rules[i].first, temp;
			TK_NTTYPES parent = gram_rules[i].nonterm_head;
			while(current != NULL)
			{
				if(current->t == TERM)
				{
					current = current->next;
				}
				else
				{
					temp = current->next;
					while(temp != NULL)
					{
						if(temp->t == NON_TERM)
						{
							int epsilon = 0;
							int p = temp->s.nonterm_type;
							j=0;
							while(j < size_of_first_set[p])
							{
								if(strcmp(map_terminals_to_strings[first[p][j]], "eps") != 0)
								{
									addToFollowSet(first[p][j],current->s.nonterm_type);
								}
								else
								{
									epsilon=1;
								}	
								j++;
							}

							if(epsilon)
							{
								temp=temp->next;
							}
							else
							{
								break;
							}
						}
						else
						{	
							addToFollowSet(temp->s.term_type, current->s.nonterm_type);
							break;
						}	
					}
					if(temp == NULL)
					{
						int p = parent;
						
						j=0;
						while(j < size_of_follow_set[p])
						{
							addToFollowSet(follow[p][j],current->s.nonterm_type);
							j++;
						}
					}
					current = current->next;
				}
			}
			i++;
		}
	}
}

void createParsingTable(){

	int i, j, k, flag;
	GrNode temp;
	for(i = 0; i < TOTAL_NUM_NT; i++){
		for(j = 0; j < TOTAL_NUM_TOKENS; j++){
			ParseTable[i][j] = -1;
		}
	}
	for(i = 0; i < RULE_COUNT; i++){
		temp = gram_rules[i].first;
		while(temp != NULL){
			flag = 0;
			if(temp->t == TERM){
				if(temp->s.term_type!=TK_EPS)
				{	
					ParseTable[gram_rules[i].nonterm_head][temp->s.term_type] = i;
					break;
				}
				else
				{
					k = 0;
					while(follow[gram_rules[i].nonterm_head][k] != -1)
					{
						ParseTable[gram_rules[i].nonterm_head][follow[gram_rules[i].nonterm_head][k]] = i;
						k++;
					}
					break;
				}	
			}
			else{
				j = 0;
				while(first[temp->s.nonterm_type][j] != -1){
					if(first[temp->s.nonterm_type][j] != TK_EPS){
						ParseTable[gram_rules[i].nonterm_head][first[temp->s.nonterm_type][j]] = i;
					}
					else{
						flag++;
					}
					j++;
				}
			}
			if(!flag){
				break;
			}
			temp = temp->next;
		}
		if(temp == NULL){
			k = 0;
			while(follow[gram_rules[i].nonterm_head][k] != -1){
				ParseTable[gram_rules[i].nonterm_head][follow[gram_rules[i].nonterm_head][k]] = i;
				k++;
			}
		}
	}
}

void printParseTable(){
	int i, j;
	for(i = 0; i < TOTAL_NUM_NT; i++){
		for(j = 0; j < TOTAL_NUM_TOKENS; j++){
			printf("%d ", ParseTable[i][j]);
		}
		printf("\n");
	}
}

void printTree(TreeNode root)
{
	if(root==NULL)
		return;
	printTree(root->child);
	AttNode gramnode=root->grammar_node;
	if(gramnode->t==TERM){
		if(gramnode->s.term_type == TK_EPS){
			return;
		}
		if(gramnode->s.term_type == TK_NUM || gramnode->s.term_type == TK_RNUM){
			printf("%43s%43d%43s%43s%43s%43s%43s\n", root->value, root->line, map_terminals_to_strings[gramnode->s.term_type], root->value, map_nonterminals_to_strings[root->parent->grammar_node->s.nonterm_type], "yes", "----");
		}
		else{
			printf("%43s%43d%43s%43s%43s%43s%43s\n", root->value, root->line, map_terminals_to_strings[gramnode->s.term_type], "----", map_nonterminals_to_strings[root->parent->grammar_node->s.nonterm_type], "yes", "----");
		}
	}
	else{
		if(root->parent != NULL){
			printf("%43s%43s%43s%43s%43s%43s%43s\n", "----", "----", "----", "----", map_nonterminals_to_strings[root->parent->grammar_node->s.nonterm_type], "no", map_nonterminals_to_strings[gramnode->s.nonterm_type]);
		}
		else{
			printf("%43s%43s%43s%43s%43s%43s%43s\n", "----", "----", "----", "----", "ROOT", "no", map_nonterminals_to_strings[gramnode->s.nonterm_type]);
		}
	}

	if(root->child!=NULL)
	{
		TreeNode temp=root->child->right;
		while(temp!=NULL){
			printTree(temp);
			temp = temp->right;
		}
	}
	return;
}


void freeParseTree(TreeNode root)
{
	if(root==NULL)
		return;
	freeParseTree(root->child);
	freeParseTree(root->right);
	free(root->grammar_node);
	free(root);
	return;
}

TreeNode parse(FILE *fp){
	//initailize stack
	Stack pStack = newStack();
	//push dollar and start symbol
	//make attNode
	AttNode dollarNode = (AttNode) malloc(sizeof(struct attNode));
	dollarNode->t = TERM;
	dollarNode->s.term_type = TK_DOL;
	parseTreeMem += sizeof(dollarNode);

	AttNode startNode = (AttNode) malloc(sizeof(struct attNode));
	startNode->t = NON_TERM;
	startNode->s.nonterm_type = program;
	parseTreeMem += sizeof(startNode);

	//push
	StackNode dnode=createNode(dollarNode);
	StackNode snode=createNode(startNode);

	pStack = push(dnode, pStack);
	pStack = push(snode, pStack);
	tokenInfo token;
	StackNode tstack, push_node;
	
	//call to get the first token and start parsing
	token=getNextToken(fp);
	int rule,j;
	GrNode temp;

	while(1)
	{
		tstack=top(pStack);

		pStack=pop(pStack);
		if(token.type_of_token==NOT_KEYWORD)
		{
			if(tstack->grammar_node->t==TERM)
			{
				if(tstack->grammar_node->s.term_type==TK_DOL)
				{
					if(no_syntax_errors){
						//printf("Compiled successfully.\n");
					}
					else{
						printf("Compilation unsuccessful, syntax errors found\n");
					}					
					break;
				}		
			}
			printf("Line %d: file ended unexpectedly\n",token.line_num);
		}

		if(tstack->grammar_node->t==TERM)
		{
			if(tstack->grammar_node->s.term_type==TK_EPS)
				continue;
			
			
			if(tstack->grammar_node->s.term_type==TK_DOL)
			{
				printf("Line %d: Extra tokens occurring after main function\n",token.line_num);
				no_syntax_errors = 0;
				break;
			}
			if(token.type_of_token==tstack->grammar_node->s.term_type)
			{
				strncpy(tstack->value, token.value, MAX_FLOAT_LEN);
				tstack->line = token.line_num;
				token=getNextToken(fp);

				while(token.type_of_token == UNK_SYMB){
					no_syntax_errors = 0;
					token = getNextToken(fp);
				}

				while(token.type_of_token==NOT_TOKEN)
				{
					no_syntax_errors = 0;			
					// pStack = pop(pStack);
					token=getNextToken(fp);
				}
				continue;
			}
			else
			{
				if(tstack->grammar_node->s.term_type==TK_SEM){
					no_syntax_errors = 0;
					printf("Line %d: Semicolon missing\n", token.line_num-1);
				}
				else
				{	
					no_syntax_errors = 0;
					printf("Line %d: The token %s for lexeme %s does not match with the expected token %s\n",token.line_num,map_terminals_to_strings[token.type_of_token],token.value,map_terminals_to_strings[tstack->grammar_node->s.term_type]);
				}
			}
		}
		else
		{
			TK_NTTYPES index = tstack->grammar_node->s.nonterm_type;
			rule=ParseTable[index][token.type_of_token];
			if(rule==-1)
			{
				no_syntax_errors = 0;
				if(token.type_of_token == TK_SEM){
					pStack = pop(pStack);
					token = getNextToken(fp);
					continue;
				}
				for(j = 0;j < size_of_follow_set[index]; j++)
				{
					if(token.type_of_token == follow[index][j])
					{
						printf("Line %d: The token %s for lexeme %s does not match with the expected token of type <%s>\n",token.line_num,map_terminals_to_strings[token.type_of_token],token.value,map_nonterminals_to_strings[index]);
						break;
					}
				}
				if(j==size_of_follow_set[index])
				{	
					
					if(!goes_to_eps[index]){
						printf("Line %d: The token %s for lexeme %s does not match with the expected token of type <%s>\n",token.line_num,map_terminals_to_strings[token.type_of_token],token.value,map_nonterminals_to_strings[index]);
						token=getNextToken(fp);
					}
					else{
						continue;
					}					
					
					while(token.type_of_token == UNK_SYMB){
						token = getNextToken(fp);
					}

					while(token.type_of_token==NOT_TOKEN)
					{
						pStack = pop(pStack);
						token=getNextToken(fp);
					}
				}
			}
			else
			{
				temp = gram_rules[rule].last;
				tstack->rule_id = rule;
				TreeNode previous=NULL;
				while(temp!=NULL)
				{
					AttNode anode = (AttNode)malloc(sizeof(struct attNode));
					parseTreeMem += sizeof(anode);
					anode->t=temp->t;
					if(temp->t==TERM)
					{
						anode->s.term_type=temp->s.term_type;
					}
					else
					{
						anode->s.nonterm_type=temp->s.nonterm_type;
					}
					push_node=createNode(anode);
					push_node->parent=tstack;
					push_node->right=previous;
					push(push_node,pStack);
					previous=push_node;
					temp = temp -> prev;
				}
				tstack->child=previous;
			}
		}
	}
	free(dollarNode);
	free(dnode);
	free(pStack);
	return snode;
}