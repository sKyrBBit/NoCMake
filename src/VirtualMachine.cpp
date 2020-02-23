#include "VirtualMachine.h"
#include <utility>
#include <string>

inline void VirtualMachine::push(uint32_t i) {
  stack.push_back(i);
  stack_pointer++;
}
inline uint32_t VirtualMachine::pop() {
  auto tmp = stack.back();
  stack.pop_back();
  stack_pointer--;
  return tmp;
}
void VirtualMachine::execute() {
  // direct threading
#define INIT_DISPATCH JUMP;
#define CASE(op) L_ ## op:
#define NEXT i=*++pc; goto *table[i.type]
#define JUMP i=*pc; goto *table[i.type]
#define END_DISPATCH L_END:
  auto pc = reinterpret_cast<instruction*>(&value_placeholder[entry_point]);
  instruction i;
  static void* table[] = { // TODO IOUT IIN
    /* 00 */ &&L_NOP,   /* 01 */ &&L_EXIT, /* 02 */ &&L_BP,   /* 03 */ &&L_NOP,
    /* 04 */ &&L_STORE, /* 05 */ &&L_LOAD, /* 06 */ &&L_PUSH, /* 07 */ &&L_POP,
    /* 08 */ &&L_COPY,  /* 09 */ &&L_NOP,  /* 0a */ &&L_SOUT, /* 0b */ &&L_CASL,
    /* 0c */ &&L_ADDL,  /* 0d */ &&L_SUBL, /* 0e */ &&L_ANDL, /* 0f */ &&L_ORL,

    /* 10 */ &&L_ADDR,  /* 11 */ &&L_SUBR, /* 12 */ &&L_MULR, /* 13 */ &&L_DIVR,
    /* 14 */ &&L_ADDI,  /* 15 */ &&L_SUBI, /* 16 */ &&L_MULI, /* 17 */ &&L_DIVI,
    /* 18 */ &&L_GT,    /* 19 */ &&L_GE,   /* 1a */ &&L_EQ,   /* 1b */ &&L_CONST,
    /* 1c */ &&L_AND,   /* 1d */ &&L_OR,   /* 1e */ &&L_NOT,  /* 1f */ &&L_NOP,

    /* 20 */ &&L_GOTO,  /* 21 */ &&L_BACK, /* 22 */ &&L_NOP,  /* 23 */ &&L_NOP,
    /* 24 */ &&L_IFGT,  /* 25 */ &&L_IFGE, /* 26 */ &&L_IFEQ, /* 27 */ &&L_IOUT,
    /* 28 */ &&L_IIN,   /* 29 */ &&L_NEW,  /* 2a */ &&L_SET,  /* 2b */ &&L_GET,
    /* 2c */ &&L_CALL,  /* 2d */ &&L_RET,  /* 2e */ &&L_NOP,  /* 2f */ &&L_NOP,

	/* 30 */ &&L_SHLR,  /* 31 */ &&L_SHRR, /* 32 */ &&L_NOP,  /* 33 */ &&L_NOP,
	/* 34 */ &&L_SHLI,  /* 35 */ &&L_SHRI, /* 36 */ &&L_NOP,  /* 37 */ &&L_NOP,
  };
  INIT_DISPATCH {
    CASE(NOP) {
    } NEXT;
    CASE(EXIT) {
      goto L_END;
    } NEXT;
    CASE(BP) {
    // TODO
      pc++;
      printf("debug | type: %x operand0: %x operand1: %x operand2: %x\n",
        pc->type, pc->operand0, pc->operand1, pc->operand2);
    } NEXT;
    CASE(STORE) {
      stack[base_pointer + i.operand0] = registers[i.operand1];
    } NEXT;
    CASE(LOAD) {
      registers[i.operand0] = stack[base_pointer + i.operand1];
    } NEXT;
    CASE(PUSH) {
      push(registers[i.operand0]);
    } NEXT;
    CASE(POP) {
      registers[i.operand0] = pop();
    } NEXT;
    CASE(COPY) {
      registers[i.operand0] = registers[i.operand1];
    } NEXT;
    CASE(SOUT) {
      cout << reinterpret_cast<char*>(&value_placeholder[i.operand0]);
    } NEXT;
    CASE(CASL) {
      __sync_bool_compare_and_swap(&registers[i.operand0], registers[i.operand1], registers[i.operand2]);
    } NEXT;
    CASE(ADDL) {
      __sync_fetch_and_add(&registers[i.operand0], registers[i.operand1]);
    } NEXT;
    CASE(SUBL) {
      __sync_fetch_and_sub(&registers[i.operand0], registers[i.operand1]);
    } NEXT;
    CASE(ANDL) {
      __sync_fetch_and_and(&registers[i.operand0], registers[i.operand1]);
    } NEXT;
    CASE(ORL) {
      __sync_fetch_and_or(&registers[i.operand0], registers[i.operand1]);
    } NEXT;
    CASE(ADDR) {
      registers[i.operand0] = registers[i.operand1] + registers[i.operand2];
    } NEXT;
    CASE(SUBR) {
      registers[i.operand0] = registers[i.operand1] - registers[i.operand2];
    } NEXT;
    CASE(MULR) {
      registers[i.operand0] = registers[i.operand1] * registers[i.operand2];
    } NEXT;
    CASE(DIVR) {
      registers[i.operand0] = registers[i.operand1] / registers[i.operand2];
    } NEXT;
	CASE(ADDI) {
      registers[i.operand0] = registers[i.operand1] + i.operand2;
    } NEXT;
    CASE(SUBI) {
      registers[i.operand0] = registers[i.operand1] - i.operand2;
    } NEXT;
    CASE(MULI) {
      registers[i.operand0] = registers[i.operand1] * i.operand2;
    } NEXT;
    CASE(DIVI) {
      registers[i.operand0] = registers[i.operand1] / i.operand2;
    } NEXT;
    CASE(GT) {
      registers[i.operand0] = registers[i.operand1] > registers[i.operand2];
    } NEXT;
    CASE(GE) {
      registers[i.operand0] = registers[i.operand1] >= registers[i.operand2];
    } NEXT;
    CASE(EQ) {
      registers[i.operand0] = registers[i.operand1] == registers[i.operand2];
    } NEXT;
    CASE(AND) {
      registers[i.operand0] = registers[i.operand1] && registers[i.operand2];
    } NEXT;
    CASE(OR) {
      registers[i.operand0] = registers[i.operand1] || registers[i.operand2];
    } NEXT;
    CASE(NOT) {
      registers[i.operand0] = !registers[i.operand1];
    } NEXT;
    CASE(CONST) {
      registers[i.operand0] = i.operand1;
    } NEXT;
    CASE(IOUT) {
      cout << registers[i.operand0];
    } NEXT;
    CASE(IIN) {
      cin >> registers[i.operand0];
    } NEXT;
    CASE(GOTO) {
      pc += i.operand0;
    } JUMP;
    CASE(BACK) {
      pc -= i.operand0;
    } JUMP;
    CASE(IFGT) {
      if (registers[i.operand1] > registers[i.operand2]) pc += i.operand0;
      else pc++;
    } JUMP;
    CASE(IFGE) {
      if (registers[i.operand1] >= registers[i.operand2]) pc += i.operand0;
      else pc++;
    } JUMP;
    CASE(IFEQ) {
      if (registers[i.operand1] == registers[i.operand2]) pc += i.operand0;
      else pc++;
    } JUMP;
    CASE(NEW) {
      heap.push_back(registers[1]); // TODO back() can be available only if the vector isn't empty.
      auto tmp = &heap.back();
      for (uint32_t u = 0; u < i.operand1; u++) heap.push_back(0u);
      registers[i.operand0] = *++tmp;
    } NEXT;
    CASE(SET) {
      reinterpret_cast<uint32_t*>(registers[i.operand0])[i.operand1] = registers[i.operand2];
    } NEXT;
    CASE(GET) {
      registers[i.operand0] = reinterpret_cast<uint32_t*>(registers[i.operand1])[i.operand2];
    } NEXT;
    CASE(CALL) {
      // TODO args
      // base pointer
      push(base_pointer);
      base_pointer = stack_pointer;
      // locals
      for (uint32_t u = 0u; u < i.operand2; u++) push(0u);
      // return address
      push(*reinterpret_cast<uint32_t*>(&++pc));
      pc = reinterpret_cast<instruction*>(&value_placeholder[i.operand1]);
    } JUMP;
    CASE(RET) {
      // return address
      pc = reinterpret_cast<instruction*>(pop());
      // locals
      stack_pointer = base_pointer;
      // base pointer
      base_pointer = pop();
    } JUMP;
	CASE(SHLR) {
	  registers[i.operand0] = registers[i.operand1] << registers[i.operand2];
	} NEXT;
	CASE(SHRR) {
	  registers[i.operand0] = registers[i.operand1] >> registers[i.operand2];
	} NEXT;
	CASE(SHLI) {
      registers[i.operand0] = registers[i.operand1] << i.operand2;
	} NEXT;
	CASE(SHRI) {
	  registers[i.operand0] = registers[i.operand1] << i.operand2;
	} NEXT;
  } END_DISPATCH;
}
#undef INIT_DISPATCH
#undef CASE
#undef NEXT
#undef JUMP
#undef END_DISPATCH