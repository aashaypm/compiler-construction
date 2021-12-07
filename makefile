exe: driver.o hashtable.o lexer.o parser.o stackTree.o astTree.o ast.o symbolTable.o typechecker.o semanticanalyzer.o codegen.o
	gcc -o compiler driver.o hashtable.o lexer.o parser.o stackTree.o astTree.o ast.o symbolTable.o typechecker.o semanticanalyzer.o codegen.o

driver.o: lexerDef.h lexer.h parserDef.h parser.h stackTree.h driver.c
	gcc -c -g driver.c -o driver.o

hashtable.o: lexerDef.h hashtable.c
	gcc -c -g hashtable.c -o hashtable.o

lexer.o: lexerDef.h lexer.c
	gcc -c -g lexer.c -o lexer.o

parser.o: lexerDef.h stackTree.h lexer.h parserDef.h parser.c
	gcc -c -g parser.c -o parser.o

stackTree.o: lexerDef.h lexer.h parserDef.h stackTree.h stackTree.c
	gcc -c -g stackTree.c -o stackTree.o

astTree.o: stackTree.h
	gcc -c -g astTree.c -o astTree.o

ast.o: stackTree.h
	gcc -c -g ast.c -o ast.o

symbolTable.o: symbolDef.h symbolTable.h
	gcc -c -g symbolTable.c -o symbolTable.o

typechecker.o: symbolTable.h
	gcc -c -g typechecker.c -o typechecker.o

semanticanalyzer.o: semanticanalyzer.h
	gcc -c -g semanticanalyzer.c -o semanticanalyzer.o

codegen.o: codegen.h
	gcc -c -g codegen.c -o codegen.o

clean:
	rm -rf *.o