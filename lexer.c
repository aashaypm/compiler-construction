/*
GROUP NUMBER: 45
GROUP MEMBERS:
	1. SHAH NEEL KAUSHIK
	2. MEHTA AASHAY PINKESH
	3. RANADE SHUBHANKAR PRASAD
*/

#include"lexerDef.h"

char current_buffer[BUF_SIZE+1];
char *curr_pos = current_buffer;
FILE* fp;
int line_no = 1, is_file_over = 0;
extern HashTable ht;

char* keywords[NUM_KEYWORDS] = {
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

FILE* getStream(FILE *fp){

	if(is_file_over==1)
	{
		rewind(fp);
		memset(current_buffer, 0, BUF_SIZE + 1);
		// fclose(fp);
		is_file_over = 0;
		line_no = 1;
		return NULL;
	}
	int read;
	
	//clear current buffer before reading into it
	memset(current_buffer, 0, BUF_SIZE + 1);

	read = fread(current_buffer, sizeof(char), BUF_SIZE, fp);
	if (read<BUF_SIZE)
	{	
		is_file_over=1;
		// fclose(fp);
		rewind(fp);
		// fclose(fp);
		// fseek(fp, 0, SEEK_SET);
	}

	curr_pos = current_buffer;

	return fp;
}

void removeComments(char *testCaseFile, char *cleanFile){
	FILE* fp_dirty = fopen(testCaseFile, "r");
	FILE* fp_clean = fopen(cleanFile, "w+");
	char buf[BUF_SIZE], buf_write[BUF_SIZE];
	int i, j, read;
	int fl = 0;
	while(!feof(fp_dirty)){
		memset(buf_write, '\0', BUF_SIZE);
		read = 0;
		if((read = fread(buf, sizeof(char), BUF_SIZE, fp_dirty)) == -1){
			printf("Error occurred while reading");
			break;
		}
		j = 0;
		for(i = 0; i < read; i++){
			if(buf[i] == '%'){
				fl++;
				continue;
			}
			else if(buf[i] == '\n' && fl){
				buf_write[j] = buf[i];
				j++;
				fl--;
				continue;
			}
			else if(fl == 1){
				continue;
			}
			else{
				buf_write[j] = buf[i];
				j++;
			}
		}
		if(fwrite(buf_write, sizeof(char), j, fp_clean) == -1){
			printf("Error ocurred while writing");
			break;
		}
	}
	fclose(fp_dirty);
	fclose(fp_clean);
	return;
}

void print_lex_error(char *value, int type_of_error){

	/*type_of_error = 0: unrecognised symbol
	                  1: wrong pattern
	                  2: lexeme length limit exceeded for id/recordid
	                  3: lexeme length limit exceeded for funid
	*/
	if(type_of_error == 0){
		printf("Line %d: Unknown Symbol %s\n", line_no, value);
		return;
	}
	else if(type_of_error == 1){
		printf("Line %d: Unknown pattern %s\n", line_no, value);
		return;
	}
	else if(type_of_error == 2){
		printf("Line %d: Identifier is longer than the prescribed length of 20 characters\n", line_no);
		return;
	}
	else if(type_of_error == 3){
		printf("Line %d: Identifier is longer than the prescribed length of 30 characters\n", line_no);
		return;
	}
	else if(type_of_error == 4){
		printf("Line %d: Number is too long\n", line_no);
	}
}

tokenInfo getNextToken(FILE *fp){
	
	tokenInfo tk_eof;
	tk_eof.type_of_token = NOT_KEYWORD;
	tk_eof.line_num = line_no;

	char *ch = curr_pos;
	if(*curr_pos == '\0'){
		fp = getStream(fp);
		if(fp == NULL)
			return tk_eof;
		ch = current_buffer;
		curr_pos = ch;
	}

	tokenInfo tk_info;
	tk_info.type_of_token = NOT_TOKEN;
	tk_info.line_num = line_no;

	int state = 0, vind = 0;
	char value[MAX_FLOAT_LEN + 1];

	while(1){
		if(*ch == '\0'){
			fp = getStream(fp);
			if(fp == NULL)
				return tk_eof;
			ch = current_buffer;
			curr_pos = ch;
		}
		
		switch(state){
			case 0:
				
				//resetting value
				memset(value, 0, MAX_LEXEME_SIZE);
				vind = 0;
				value[vind++]=*ch;

				switch(*ch){
					case '-':
						state = 1;
						break;
					
					case '*':
						state = 2;
						break;
					
					case '~':
						state = 3;
						break;

					case '!':
						state = 4;
						break;

					case ':':
						state = 6;
						break;

					case '[':
						state = 7;
						break;

					case ']':
						state = 8;
						break;

					case '<':
						state = 9;
						break;

					case '=':
						state = 15;
						break;

					case '&':
						state = 17;
						break;

					case '#':
						state = 20;
						break;

					case '0' ... '9':
						state = 23;
						break;

					case '_':
						state = 28;
						break;

					case 'b' ... 'd':
						state = 34;
						break;

					case 'a':
					case 'e' ... 'z':
						state = 32;
						break;

					case '@':
						state = 39;
						break;

					case '\n':
					// case '\r':
						line_no++;
						break;

					case '%':
						state = 43;
						break;

					case '>':
						state = 45;
						break;

					case ')':
						state = 48;
						break;

					case '(':
						state = 49;
						break;

					case ';':
						state = 50;
						break;

					case ',':
						state = 51;
						break;

					case '.':
						state = 52;
						break;

					case '/':
						state = 53;
						break;

					case '+':
						state = 54;
						break;

					case ' ':
					case '\t':
						state = 0;
						break;					

					default:
						// value[vind++] = *ch;
						value[vind] = '\0';
						ch++;
						curr_pos = ch;
						print_lex_error(value, 0);
						tk_info.type_of_token = UNK_SYMB;
						tk_info.line_num = line_no;
						return tk_info;
				}
				break;

			case 1:
				tk_info.type_of_token = TK_MINUS;
				tk_info.line_num = line_no;			
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 2:
				tk_info.type_of_token = TK_MUL;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 3:
				tk_info.type_of_token = TK_NOT;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 4:
				if(*ch == '='){
					state = 5;
					value[vind++]=*ch;
				}
				else{					
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 5:
				tk_info.type_of_token = TK_NE;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 6:
				tk_info.type_of_token = TK_COLON;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 7:
				tk_info.type_of_token = TK_SQL;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 8:
				tk_info.type_of_token = TK_SQR;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 9:											//also state 10 (retract)
				if(*ch == '='){
					value[vind++] = *ch;
					state = 14;
				}
				else if(*ch == '-'){
					value[vind++] = *ch;
					state = 11;
				}
				else{
					tk_info.type_of_token = TK_LT;
					tk_info.line_num = line_no;					
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;							

			case 11:
				if(*ch == '-'){
					value[vind++] = *ch;
					state = 12;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 12:			
				if(*ch == '-'){
					value[vind++] = *ch;
					state = 13;
				}
				else{					
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 13:
				tk_info.type_of_token = TK_ASSIGNOP;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 14:
				tk_info.type_of_token = TK_LE;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 15:				
				if(*ch == '='){
					value[vind++] = *ch;
					state = 16;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 16:
				tk_info.type_of_token = TK_EQ;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 17:				
				if(*ch == '&'){
					value[vind++] = *ch;
					state = 18;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 18:				
				if(*ch == '&'){
					value[vind++] = *ch;
					state = 19;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 19:
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				tk_info.type_of_token = TK_AND;
				tk_info.line_num = line_no;
				curr_pos = ch;
				return tk_info;

			case 20:				
				if(*ch >= 'a' && *ch <= 'z'){
					value[vind++] = *ch;
					state = 21;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 21:													//also state 22 (retract state)
				if((*ch >= 'a' && *ch <= 'z') && vind >= 20){
					while(*ch >= 'a' && *ch <= 'z'){
						ch++;
						if(*ch == '\0'){
							fp = getStream(fp);
							if(fp == NULL)
								return tk_eof;
							ch = current_buffer;
							curr_pos = ch;
						}
					}
					value[19] = '\0';
					print_lex_error(value, 2);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				else if(*ch >= 'a' && *ch <= 'z'){
					value[vind] = *ch;
					vind++;
					state = 21;
				}				
				else
				{
					tk_info.type_of_token = TK_RECORDID;
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}										
				break;			

			case 23:													//also state 27 (retract state)
				if(*ch >= '0' && *ch <= '9' && vind < 39){
					value[vind] = *ch;
					vind++;
				}
				else if(vind >= 39){
					while((*ch >= '0' && *ch <= '9') || *ch == '.'){
						ch++;
						if(*ch == '\0'){
							fp = getStream(fp);
							if(fp == NULL)
								return tk_eof;
							ch = current_buffer;
							curr_pos = ch;
						}
					}
					value[39] = '\0';
					print_lex_error(value, 4);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					// tk_info.type_of_token = TK_NUM;
					tk_info.line_num = line_no;
					curr_pos = ch;
					return tk_info;
				}
				else if(*ch == '.'){
					value[vind] = *ch;
					vind++;
					state = 24;
				}
				else if (vind >= 10){					
					value[11] = '\0';
					print_lex_error(value, 4);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					tk_info.type_of_token = TK_NUM;
					tk_info.line_num = line_no;
					curr_pos = ch;
					return tk_info;
				}
				else{
					tk_info.type_of_token = TK_NUM;
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);					
					curr_pos = ch;
					return tk_info;
				}
				break;

			case 24:
				if(*ch >= '0' && *ch <= '9'){
					value[vind] = *ch;
					vind++;
					state = 25;
				}
				else{					
					value[vind] = '\0';
					print_lex_error(value, 1);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				break;

			case 25:
				if(*ch >= '0' && *ch <= '9'){
					value[vind] = *ch;
					vind++;
					state = 26;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 26:
				if(*ch >= '0' && *ch <= '9'){
					while(*ch >= '0' && *ch <= '9'){
						ch++;
						if(*ch == '\0'){
							fp = getStream(fp);
							if(fp == NULL)
								return tk_eof;
							ch = current_buffer;
							curr_pos = ch;
						}
					}
					value[MAX_FLOAT_LEN] = '\0';
					print_lex_error(value, 4);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					// tk_info.type_of_token = TK_RNUM;
					tk_info.line_num = line_no;
					curr_pos = ch;
					return tk_info;
				}
				else if(vind <= MAX_FLOAT_LEN){
					tk_info.type_of_token = TK_RNUM;
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;					
				}
				break;

			case 28:
				if((*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z')){
					value[vind++] = *ch;
					state = 29;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;					
				}
				break;

			case 29:												//also state 31 (retract state)
				if(((*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z')) && vind >= MAX_LEXEME_SIZE){
					while((*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z') || (*ch >= '0' && *ch <= '9')){
						ch++;
						if(*ch == '\0'){
							fp = getStream(fp);
							if(fp == NULL)
								return tk_eof;
							ch = current_buffer;
							curr_pos = ch;
						}
					}
					value[MAX_LEXEME_SIZE] = '\0';
					print_lex_error(value, 3);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					// tk_info.type_of_token = TK_FUNID;
					tk_info.line_num = line_no;
					return tk_info;
				}
				else if ((*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z')){
					value[vind++] = *ch;
				}
				else if(*ch >= '0' && *ch <= '9'){
					value[vind++] = *ch;
					state = 30;
				}
				else if(vind <= MAX_LEXEME_SIZE){
					tk_info.type_of_token = findKeyword(ht, value);
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					if(tk_info.type_of_token == NOT_KEYWORD){
						tk_info.type_of_token = TK_FUNID;
					}
					return tk_info;
				}
				else {
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 30:														//also state 31 (retracting)
				if((*ch >= '0' && *ch <= '9') && vind >= MAX_LEXEME_SIZE){
					while(*ch >= '0' && *ch <= '9'){
						ch++;
						if(*ch == '\0'){
							fp = getStream(fp);
							if(fp == NULL)
								return tk_eof;
							ch = current_buffer;
							curr_pos = ch;
						}
					}
					value[MAX_LEXEME_SIZE] = '\0';
					print_lex_error(value, 3);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					// tk_info.type_of_token = TK_FUNID;
					tk_info.line_num = line_no;
					return tk_info;
				}
				else if(*ch >= '0' && *ch <= '9'){
					value[vind++] = *ch;
				}
				else if(vind <= MAX_LEXEME_SIZE){
					tk_info.type_of_token = findKeyword(ht, value);
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					if(tk_info.type_of_token == NOT_KEYWORD){
						tk_info.type_of_token = TK_FUNID;
					}
					return tk_info;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				break;

			case 32:
				if(*ch >= 'a' && *ch <= 'z' && vind >= 20){
					while(*ch >= 'a' && *ch <= 'z'){
						ch++;
						if(*ch == '\0'){
							fp = getStream(fp);
							if(fp == NULL)
								return tk_eof;
							ch = current_buffer;
							curr_pos = ch;
						}
					}
					value[20] = '\0';
					print_lex_error(value, 2);
					curr_pos = ch;
					// tk_info.type_of_token = TK_FIELDID;
					tk_info.line_num = line_no;
					return tk_info;
				}
				else if(*ch >= 'a' && *ch <= 'z'){
					value[vind++] = *ch;
				}
				else if(vind <= 20){
					tk_info.type_of_token = findKeyword(ht, value);
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					if(tk_info.type_of_token == NOT_KEYWORD){
						tk_info.type_of_token = TK_FIELDID;
					}
					return tk_info;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				break;

			case 34:
				if(*ch >= '2' && *ch <= '7'){
					state = 35;
					value[vind++] = *ch;
				}
				else if(*ch >= 'a' && *ch <= 'z'){
					state = 32;
					value[vind++] = *ch;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				break;

			case 35:
				if(*ch >= '2' && *ch <= '7'){
					state = 38;
					value[vind++] = *ch;
				}
				else if(*ch >= 'b' && *ch <= 'd'){
					state = 36;
					value[vind++] = *ch;
				}
				else{
					tk_info.type_of_token = TK_ID;
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				break;

			case 36:
				if(*ch >= 'b' && *ch <= 'd' && vind >= 20){
					while((*ch >= 'b' && *ch <= 'd') || (*ch >= '2' && *ch <= '7')){
						ch++;
						if(*ch == '\0'){
							fp = getStream(fp);
							if(fp == NULL)
								return tk_eof;
							ch = current_buffer;
							curr_pos = ch;
						}
					}
					value[20] = '\0';
					print_lex_error(value, 2);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					tk_info.line_num = line_no;
					return tk_info;
				}
				else if(*ch >= 'b' && *ch <= 'd'){
					value[vind++] = *ch;
				}
				else if(*ch >= '2' && *ch <= '7'){
					state = 38;
					value[vind++] = *ch;
				}
				else if(vind <= 20){
					tk_info.type_of_token = TK_ID;
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				break;

			case 38:
				if(*ch >= '2' && *ch <= '7' && vind >= 20){
					while((*ch >= 'b' && *ch <= 'd') || (*ch >= '2' && *ch <= '7')){
						ch++;
						if(*ch == '\0'){
							fp = getStream(fp);
							if(fp == NULL)
								return tk_eof;
							ch = current_buffer;
							curr_pos = ch;
						}
					}
					value[20] = '\0';
					print_lex_error(value, 2);
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					// tk_info.type_of_token = TK_ID;
					tk_info.line_num = line_no;
					return tk_info;
				}
				else if(*ch >= '2' && *ch <= '7'){					
					value[vind++] = *ch;
				}
				else if(vind <= 20){
					tk_info.type_of_token = TK_ID;
					tk_info.line_num = line_no;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					curr_pos = ch;
					return tk_info;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;					
				}
				break;

			case 39:
				if(*ch == '@'){
					value[vind++] = *ch;
					state = 40;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 40:
				if(*ch == '@'){
					value[vind++] = *ch;
					state = 41;
				}
				else{
					value[vind] = '\0';
					print_lex_error(value, 1);
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 41:
				tk_info.type_of_token = TK_OR;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 43:
				// while(*ch != '\n' && *ch != '\r'){
				while(*ch != '\n'){
					ch++;
					if(*ch == '\0'){
						fp = getStream(fp);
						if(fp == NULL)
							return tk_eof;
						ch = current_buffer;
						curr_pos = ch;
					}
				}
				line_no++;
				state = 0;
				break;

			case 45:
				if(*ch == '='){
					value[vind++] = *ch;
					state = 47;
				}
				else{
					tk_info.type_of_token = TK_GT;
					tk_info.line_num = line_no;
					curr_pos = ch;
					strncpy(tk_info.value, value, MAX_FLOAT_LEN);
					return tk_info;
				}
				break;

			case 47:
				tk_info.type_of_token = TK_GE;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 48:
				tk_info.type_of_token = TK_CL;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 49:
				tk_info.type_of_token = TK_OP;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 50:
				tk_info.type_of_token = TK_SEM;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 51:
				tk_info.type_of_token = TK_COMMA;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 52:
				tk_info.type_of_token = TK_DOT;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 53:
				tk_info.type_of_token = TK_DIV;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;

			case 54:
				tk_info.type_of_token = TK_PLUS;
				tk_info.line_num = line_no;
				curr_pos = ch;
				strncpy(tk_info.value, value, MAX_FLOAT_LEN);
				return tk_info;
		}
		ch++;
	}
}