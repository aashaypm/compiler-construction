/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#ifndef _LEX_
#define _LEX_

FILE* getStream(FILE *fp);
tokenInfo getNextToken(FILE *fp);
void removeComments(char *testCaseFile, char *cleanFile);

#endif