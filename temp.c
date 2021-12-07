#include <stdio.h>
#include <float.h>
#include <string.h>

int main(){
	// printf("max float value = %f\n", FLT_MAX);
	/*char *buf1 = "aaaa";
	char *buf2 = "bbbb";
	printf("Buf1 = %s Buf2 = %s\n", buf1, buf2);
	char *temp = buf1;
	buf1 = buf2;
	buf2 = temp;
	printf("Buf1 = %s Buf2 = %s\n", buf1, buf2);
	printf("size of buf = %d\n", sizeof(buf1));*/
	char* keywords[24] = {
	"with",
	"parameters",
	"end",
	"while",
	"type",
	"_main",
	"global",
	"parameter",
	"list",
	"input",
	"output",
	"int",
	"real",
	"endwhile",
	"if",
	"then",
	"endif",
	"read",
	"write",
	"return",
	"call",
	"record",
	"endrecord",
	"else"
	};
	unsigned long hash;
	int c;
	char *str = "123";
	/*for (int i = 0; i < 24; i++)
	{
		hash = 5381;
		while (c = *keywords[i]++)
			hash = ((hash << 5) + hash) + c; 
		printf("sum = %d\n", (int)(hash%59));	
	}*/
	printf("number = %d\n", (int)str);

	return 0;
}