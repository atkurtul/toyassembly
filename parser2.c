#include "shared.h"

typedef struct instruction {
	enum opcode opc;
	enum operand op[2];
	union {
		u64 imm[2];
		struct label* label;
	};
} instruction;


typedef struct label {
	u64 len;
	instruction* inst;
	u64 addr;
	const char* name;
} label;


token* data_section(token* t) {

}

token* assert_punct(token* t, const char* p) {
	assert(t->tag == TOKEN_PUNCT && 
		!memcmp(t->str, p, t->slen));
	return ++t;
}

#define MATCH_OPC(OPC) \
if (!memcmp(t->str, #OPC, t->slen))\
	return OPC_##OPC;

enum opcode get_opc(token* t) {
	MATCH_OPC(POP)
	MATCH_OPC(PUSH)
	MATCH_OPC(RET)
	MATCH_OPC(CALL)
	MATCH_OPC(JMP)
	MATCH_OPC(CMP)
	MATCH_OPC(MOV)
	MATCH_OPC(ADD)
	MATCH_OPC(SUB)
	MATCH_OPC(MUL)
	MATCH_OPC(DIV)
	MATCH_OPC(XOR)
	MATCH_OPC(LOR)
	MATCH_OPC(AND)
	MATCH_OPC(NOT)
	MATCH_OPC(INC)
	MATCH_OPC(DEC)
	MATCH_OPC(HALT)
	return 0;
}

token* text_section(token* t) {
	assert(t->tag == TOKEN_IDENT);

	label lab = { 0 };
	lab.name = memcpy(calloc(t->slen + 1, 1), t->str, t->slen);
	
	t = assert_punct(++t, ":");

	assert(t->tag == TOKEN_IDENT);
	int opc = 0;
	if (!(opc = get_opc(t++))) {
		printf("Invalid opcode\n");
		exit(1);
	}
	
}

token* top_level(token* t) {

	t = assert_punct(t, "#");
	assert(t->tag == TOKEN_IDENT);

	if (!memcmp(t->str, "data", 4)) {
		t = data_section(++t);
	}
	else if (!memcmp(t->str, "text", 4)) {
		t = text_section(++t);
	}
	else goto FAIL;

	return t;

	FAIL:
		printf("Invalid token\n");
		exit(1);
	
}

int main() {

}