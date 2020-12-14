
#include "shared.h"


static char* readfile(const char* file) {
	FILE* f = fopen(file, "r");
	if (!f) {
		printf("fopen fail %s\n", file);
	}
	if (fseek(f, 0, SEEK_END)) {
		printf("fseek fail %s\n", file);
	}
	u64 size = ftell(f);
	char* buff = calloc(size + 1, 1);
	rewind(f);
	fread(buff, size, size, f);
	fclose(f);
	return buff;
}

static int getline(char** line, char** file) {
	if (**file == 0)
		return 0;
	char* p = *line = *file;
	do {
		if (*p == '\n') {
			*p++ = 0;
			break;
		}
		if (*p == EOF)  {
			*p = 0; 
			break;
		}
	} while(*p++);
	*file = p;
	return 1;
}

static char* skip_ws(char** buf) {
	while(**buf == ' ' || **buf == '\t') ++*buf;
	return *buf;
}

static int parse_ident(char* buf) {
	int len = 0;
	if (isalpha(*buf) || *buf == '_') {
		len = 1;
		while(isalnum(buf[len]) || buf[len] == '_') ++len;
	}
	return len;
}


static int parse_punct(char* buf) {
	int len = 0;
	while(ispunct(buf[len]) && buf[len] != '_') ++len;
	return len;
}

static int parse_hex(char** buf, u64* re) {
	int n = 0;
	int d = 0;
	while(-1 != (d = isdigit2(buf))) {
		++n;
		*re <<= 4;
		*re += d;
	}
	return n;
}
static int parse_int(char** buf, u64* re) {
	if (isdigit(**buf)) {
		int n = 0;
		while(isdigit(**buf)) {
			++n;
			*re *= 10;
			*re +=  *(*buf)++-'0';
		}
		return n;
	}
	return 0;
}
static int isdigit2(char** buf) {
	if (isdigit(**buf) ) 
		return *(*buf)++ - '0';
	if (**buf >= 'a' && **buf <= 'f') 
		return *(*buf)++ - 'a' + 10;
	return -1;
}

static int parse_num(char** buf, u64* re) {
	*re = 0;
	if (!memcmp(*buf, "0x", 2)) {
		*buf += 2;
		parse_hex(buf, re);
		return TOKEN_HEX;
	}
	int suc = parse_int(buf, re);
	if(**buf == '.') {
		++*buf;
		u64 frac = 0;
		int d;
		if ((d = parse_int(buf, &frac)) || suc) {
			double ff = (double)*re + (double)frac / pow(10, d);
			*re = *(u64*)&ff;
			return TOKEN_REAL;
		} else {
			printf("Failure \n");
			exit(1);
		}
	}
	if (suc) {
		return TOKEN_NUM;
	}
	return 0;
}

static token* tokenize(char* src, token* tok) {
	u64 len;
	u64 num;
	// printf("---------\n");
	// printf("Line %s\n", src);
	// printf("---------\n");
NEXT:
	skip_ws(&src);
	while(len = parse_ident(src)) {
		printf("Ident %.*s\n", len, src);
		tok->tag = TOKEN_IDENT;
		tok->str = src;
		tok->slen = len;
		src += len;
		++tok;
		goto NEXT;
	}
	while(len = parse_punct(src)) {
		printf("Punct %.*s\n", len, src);
		tok->tag = TOKEN_PUNCT;
		tok->str = src;
		tok->slen = len;
		src += len;
		++tok;
		goto NEXT;
	}
	while(len = parse_num(&src, &num)) {
		if (len == TOKEN_NUM)
			printf("Num %llu\n", num);
		if (len == TOKEN_REAL)
			printf("Num %f\n", *(double*)&num);
		if (len == TOKEN_HEX)
			printf("Num %#llx\n", num);
		tok->tag = len;
		tok->num = num;
		++tok;
		goto NEXT;
	}
	return tok;
}


int main() {
	char* file = readfile("slang.asm");
	char* line;
	token tokens[1024] = { 0 };
	token* cursor = tokens;
	while(getline(&line, &file)) {
		cursor = tokenize(line, cursor);
		cursor++->tag = TOKEN_LINE;
	}
}
