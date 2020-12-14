
#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include <ctype.h>
#include <assert.h>
#include <math.h>
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long 	i64;
typedef int 		i32;
typedef short 		i16;
typedef char 		i8;

enum operand {
	R0 = 0,
	R1 = 1,
	R2 = 2,
	R3 = 3,
	RV = 4,
	SP = 5,
	BP = 6,
	IP = 7,
	R0P = R0 + 4,
	R1P = R1 + 4,
	R2P = R2 + 4,
	R3P = R3 + 4,
	RVP = RV + 4,
	SPP = SP + 4,
	BPP = BP + 4,
	IPP = IP + 4,
	IMM16,
	IMM32,
	IMM64,
	LABEL,
};

enum opcode {
	OPC_INVALID = 0,
	OPC_POP,	//dst
	OPC_PUSH,	//src
	OPC_RET,
	OPC_CALL,	//dst
	OPC_JMP,	//pred, addr
	OPC_CMP,	//dst, src
	OPC_MOV,	//dst, src
	OPC_ADD,	//dst, src
	OPC_SUB,	//dst, src
	OPC_MUL,	//dst, src
	OPC_DIV,	//dst, src
	OPC_XOR,	//dst, src
	OPC_LOR,	//dst, src
	OPC_AND,	//dst, src
	OPC_NOT,	//dst
	OPC_INC,	//dst
	OPC_DEC,	//dst
	OPC_HALT,
	OPC_MAX,
};


typedef struct token {
	enum {
		TOKEN_IDENT,
		TOKEN_PUNCT,
		TOKEN_STR,
		TOKEN_REAL,
		TOKEN_NUM,
		TOKEN_HEX,
		TOKEN_LINE,
		TOKEN_END = 0,
	} tag;
	union {
		u64	num;
		struct {
			u64 slen;
			u8* str;
		};
	};
} token;
