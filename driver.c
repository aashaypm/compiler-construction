/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#include "lexerDef.h"
#include "lexer.h"
#include "parser.h"
#include "stackTree.h"
#include "parserDef.h"
#include "symbolTable.h"
#include "semanticanalyzer.h"
#include "codegen.h"
#include <time.h>


extern globalEntry gtable[GLOBAL_TABLE_SIZE];
extern funcEntry ftable[F_TABLE_SIZE];

extern HashTable ht;
extern char *map_terminals_to_strings[TOTAL_NUM_TOKENS];
extern int line_no;
extern int parseTreeMem;
extern int astMem;
extern int parseTreeNodes;
extern int astNodes;
extern int no_syntax_errors;
extern int errflag;

//Reads <filename> and prints all lexemes along with their respective line numbers
void printTokens(FILE *fp){
	while(1){
		tokenInfo token = getNextToken(fp);
		if(token.type_of_token == NOT_KEYWORD){
			break;
		}
		if(token.type_of_token == NOT_TOKEN || token.type_of_token == UNK_SYMB){
			// printf("--------------->\n");
			continue;
		}
		printf("Line %d: Token Name: %-15s Lexeme = %s\n", token.line_num, map_terminals_to_strings[token.type_of_token], token.value);		
	}
}

int main(int argc, char *argv[]){

	//Initializes a hash-table ADT to store keywords
	ht = initializeHashTable();
	FILE *fp = fopen(argv[1], "r");
	// FILE *outfile = fopen(argv[2], "w");
	if(argc!=3){
		printf("Format: ./compiler [inputfile] [outputfile]\n");
		return 0;
	}
	int option;
	TreeNode root;
	clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
	AstNode astRoot;

    printf("Level: 4\nSymbol Table / Type Checking / Semantic Analyzer / Code Generator (Except for I/O statements) Modules Work.\n\n");
	while(1){
		printf("Enter option: ");
		scanf("%d", &option);
		switch(option){
			line_no = 1;
			//Press 0 to exit the program
			case 0:
				fclose(fp);
				// fclose(outfile);
				freeHashTable();
				return 0;

			//Press 1 to create a copy of the source code without any comments
			case 1:
				/*removeComments(argv[1], "no_comments.txt");
				system("cat no_comments.txt");
				break;*/
				printTokens(fp);
				if(feof(fp)){
					printf("Rewinding1\n");
					rewind(fp);
				}
				break;

			//Press 2 to print a list of all lexemes in the source file
			case 2:
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				parseTreeMem = 0;
				parseTreeNodes = 0;
				root = parse(fp);
				printTree(root);
				break;
			
			case 3:
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				parseTreeMem = 0;      //reset memory and number of nodes for parsetree
				parseTreeNodes = 0;				
				root = parse(fp);
				astMem = 0;           //reset memory and number of nodes for ast
				astNodes = 0;
				if (!no_syntax_errors) break;
				createAst(root);
				astRoot = root->addr;
				// freeParseTree(root);
				printf("Preorder traversal\n");
				printAst(astRoot);
				
				//freeParseTree(root);
				//freeGrammar();
				break;

			case 4:	
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				parseTreeMem = 0;      //reset memory and number of nodes for parsetree
				parseTreeNodes = 0;				
				root = parse(fp);
				astMem = 0;           //reset memory and number of nodes for ast
				astNodes = 0;
				if (!no_syntax_errors) break;
				createAst(root);
				astRoot = root->addr;
				printf("Parse Tree number of nodes = %d;\tParse Tree Allocated memory = %d\n", parseTreeNodes, parseTreeMem);
				printf("AST number of nodes = %d;\tAST Allocated memory = %d\n", astNodes, astMem);
				printf("Compression percentage = ((%d - %d)/%d)*100 = %f\n",parseTreeMem, astMem, parseTreeMem, ((float)(parseTreeMem-astMem)/parseTreeMem)*100);

				break;			
			    /*start_time = clock();
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				root = parse(fp);
				printTree(root, &outfile);
				freeParseTree(root);
				freeGrammar();
				end_time = clock();
			    total_CPU_time  =  (double) (end_time - start_time);
			    total_CPU_time_in_seconds =   total_CPU_time / CLOCKS_PER_SEC;
			    printf("time taken = %fs, ticks = %f\n", total_CPU_time_in_seconds, total_CPU_time);
				break;*/

			case 5:
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				parseTreeMem = 0;      //reset memory and number of nodes for parsetree
				parseTreeNodes = 0;				
				root = parse(fp);
				astMem = 0;           //reset memory and number of nodes for ast
				astNodes = 0;
				if (!no_syntax_errors) break;				
				createAst(root);
				astRoot = root->addr;

				populateSymbolTables(astRoot);
				printSymbolTable();
				if(!errflag)
					printf("compiled successfully\n");
				break;

			case 6:
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				root = parse(fp);
				if (!no_syntax_errors) break;
				createAst(root);
				astRoot = root->addr;
				populateSymbolTables(astRoot);
				printGlobalTable();
				break;

			case 7:
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				root = parse(fp);
				if (!no_syntax_errors) break;
				createAst(root);
				astRoot = root->addr;
				populateSymbolTables(astRoot);
				printFuncMems();
				break;

			case 8:
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				root = parse(fp);
				if (!no_syntax_errors) break;
				createAst(root);
				astRoot = root->addr;
				populateSymbolTables(astRoot);
				printRecordTable();
				break;

			case 9:
				start_time = clock();
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				root = parse(fp);
				if(!no_syntax_errors) break;
				createAst(root);
				astRoot = root->addr;
				//printTree(root, &outfile);
				//freeParseTree(root);
				//freeGrammar();
				populateSymbolTables(astRoot);
				propagateType(astRoot);
				semanticAnalyzer(astRoot);
				if(!errflag)
					printf("compiled successfully\n");
				end_time = clock();
			    total_CPU_time  =  (double) (end_time - start_time);
			    total_CPU_time_in_seconds =   total_CPU_time / CLOCKS_PER_SEC;
			    printf("time taken = %fs, ticks = %f\n", total_CPU_time_in_seconds, total_CPU_time);
				break;

			case 10:
				loadGrammar();
				computeFirstSet();
				computeFollowSet();
				createParsingTable();
				root = parse(fp);
				if (!no_syntax_errors) break;
				createAst(root);
				astRoot = root->addr;
				populateSymbolTables(astRoot);
				propagateType(astRoot);
				semanticAnalyzer(astRoot);
				if(!errflag){
					printf("compiled successfully\n");
					codeGenerator(astRoot, gtable, ftable, argv[2]);
				}
				break;


			default:
				printf("Not a valid option\n");
		}
	}
	return 0;
}