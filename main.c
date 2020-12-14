#include "shared.h"

typedef struct PROG {
	u8 	TEXT[1024];
	u64 STACK[];
} PROG;


typedef struct CPU {
	union { u64 r0; u32 s0; };
	union { u64 r1; u32 s1; };
	union { u64 r2; u32 s2; };
	union { u64 r3; u32 s3; };
	u64 rv;
	u64 sp;
	u64 bp;
	u64 ip;
	u64 imm;
} CPU;
/*
	r0 0	[r0] 4		rv 8	[rv] c
	r1 1	[r1] 5		sp 9	[sp] d
	r2 2	[r2] 6		bp a	[bp] e
	r3 3	[r3] 7		ip b	[ip] f
*/


u64 fetch(CPU* cpu, PROG* prog) {
	return prog->TEXT[cpu->ip++];
}

u64 fetch16(CPU* cpu, PROG* prog) {
	u16 re = fetch(cpu, prog);
	return re | (fetch(cpu, prog) << 8);
}

u64* operand(CPU* cpu, PROG* prog) {
	u64 r = fetch(cpu, prog);
	cpu->imm = 0;
	if (r >= IMM16) {
		cpu->imm |= fetch16(cpu, prog);
		if (r >= IMM32) {
			cpu->imm |= fetch16(cpu, prog) << 16;
		}
		if (r >= IMM64) {
			cpu->imm |= fetch16(cpu, prog) << 32;
			cpu->imm |= fetch16(cpu, prog) << 48;
		}
		return &cpu->imm;
	}
	u64* reg = &cpu->r0 + r;
	if(r < R0P)
		return reg;
	return (u64*)&prog->TEXT[*reg];
}


#define BINOP(op) \
lhs = operand(cpu, prog);\
rhs = operand(cpu, prog);\
*lhs op *rhs;\
goto NEXT;

#define UNOP(op) \
lhs = operand(cpu, prog);\
*lhs op *lhs;\
goto NEXT;

const char* inst_str(u8 inst) {
	switch(inst) {
		case OPC_POP: return "POP";
		case OPC_PUSH: return "PUSH";
		case OPC_RET: return "RET";
		case OPC_CALL: return "CALL";
		case OPC_JMP: return "JMP";
		case OPC_CMP: return "CMP";
		case OPC_MOV: return "MOV";
		case OPC_ADD: return "ADD";
		case OPC_SUB: return "SUB";
		case OPC_MUL: return "MUL";
		case OPC_DIV: return "DIV";
		case OPC_XOR: return "XOR";
		case OPC_LOR: return "LOR";
		case OPC_AND: return "AND";
		case OPC_NOT: return "NOT";
		case OPC_INC: return "INC";
		case OPC_DEC: return "DEC";
		case OPC_HALT: return "HALT";
		default: return "invalid opcode";
	}
}

void execute(CPU* cpu, PROG* prog) {
	static void* table[OPC_MAX] = {
			[OPC_POP]	= &&POP,
			[OPC_PUSH]	= &&PUSH,
			[OPC_RET]	= &&RET,
			[OPC_CALL]	= &&CALL,
			[OPC_JMP]	= &&JMP,
			[OPC_CMP]	= &&CMP,
			[OPC_MOV]	= &&MOV,
			[OPC_ADD]	= &&ADD,
			[OPC_SUB]	= &&SUB,
			[OPC_MUL]	= &&MUL,
			[OPC_DIV]	= &&DIV,
			[OPC_XOR]	= &&XOR,
			[OPC_LOR]	= &&LOR,
			[OPC_AND]	= &&AND,
			[OPC_NOT]	= &&NOT,
			[OPC_INC]	= &&INC,
			[OPC_DEC]	= &&DEC,
			[OPC_HALT]	= &&HALT,
		};

	u64	*lhs, *rhs;
	u64 addr;
	NEXT:
		u8 inst = fetch(cpu, prog);
		printf("0x%05llX %5s    0x%02llX\n",  cpu->ip - 1, inst_str(inst), cpu->sp * 8);
		goto *table[inst];
	POP:
		*operand(cpu, prog) = prog->STACK[- --cpu->sp];
		goto NEXT;
	PUSH:
		prog->STACK[-++cpu->sp] = *operand(cpu, prog);
		goto NEXT;
	RET:
		cpu->ip = prog->STACK[- --cpu->sp];
		goto NEXT;
	CALL:
		addr = *operand(cpu, prog);
		prog->STACK[-cpu->sp++] = cpu->ip;
		cpu->ip = addr;
		goto NEXT;
	JMP:
		u64 pred = *operand(cpu, prog) != 0;
		addr = *operand(cpu, prog);
		cpu->ip = cpu->ip * !pred + addr * pred;
		goto NEXT;
	CMP: BINOP(=*lhs==)
	MOV: BINOP(=)
	ADD: BINOP(+=)
	SUB: BINOP(-=)
	MUL: BINOP(*=)
	DIV: BINOP(/=)
	XOR: BINOP(^=)
	LOR: BINOP(|=)
	AND: BINOP(&=)
	NOT: UNOP(=~)
	INC: UNOP(=1+)
	DEC: UNOP(=-1+)
	HALT: return;
}


int main() {
	
	CPU cpu = { 0 };


#define ADDR16(fn) IMM16, ((u8*)&fn)[0], ((u8*)&fn)[1]

#define _size(fn) (sizeof((u8[]){fn}))

#define _fact\
	OPC_MOV, R1, R0,\
	OPC_INC, R1,\
	OPC_MUL, R0, R1,\
	OPC_DIV, R0, IMM16, 0x2, 0x0,\
	OPC_RET

#define _main \
	OPC_MOV, R0, IMM16, 0x4, 0x0,\
	OPC_CALL, ADDR16(fact),\
	OPC_HALT

	u16 fact = _size(_main);

	PROG prog = { 
		.TEXT = { 
			_main, 
			_fact,
		} 
	};

	execute(&cpu, &prog);
	printf("%llu\n", cpu.r0);
}
