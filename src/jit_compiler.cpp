#include "VirtualMachine.h"
#include <dlfcn.h>

uint32_t VirtualMachine::jit_compile(vm_function_attribute* attribute) {
  string jit = string("tmp/");
  jit += attribute->name;
  jit += ".c";
  ofstream fout(jit.c_str(), ios::out | ios::trunc);
  fout << "#include <stdio.h>" << endl
       << attribute->type.c_str()
       << " "
       << attribute->name.c_str()
       << "(void) {"
       << endl
       << "  long r0, r1, r2, r3, r4, r5, r6, r7, r8;"
       << endl;

  // direct threading
#define INIT_DISPATCH JUMP;
#define CASE(op) L_ ## op:
#define NEXT if (u++ < attribute->size) { i=*++pc; goto *table[i.type]; } else { goto L_END; }
#define JUMP if (u++ < attribute->size) { i= *pc; goto *table[i.type]; } else { goto L_END; }
#define END_DISPATCH L_END:

  uint32_t u = 0u;
  auto* pc = attribute->body;
  instruction i;
  static void* table[] = {
    /* 00 */ &&L_NOP,   /* 01 */ &&L_EXIT, /* 02 */ &&L_DEBUG, /* 03 */ &&L_NOP,
    /* 04 */ &&L_STORE, /* 05 */ &&L_LOAD, /* 06 */ &&L_PUSH,  /* 07 */ &&L_POP,
    /* 08 */ &&L_COPY,  /* 09 */ &&L_NOP,  /* 0a */ &&L_SOUT,  /* 0b */ &&L_CASL,
    /* 0c */ &&L_ADDL,  /* 0d */ &&L_SUBL, /* 0e */ &&L_ANDL,  /* 0f */ &&L_ORL,

    /* 10 */ &&L_ADD,   /* 11 */ &&L_SUB,  /* 12 */ &&L_MUL,   /* 13 */ &&L_DIV,
    /* 14 */ &&L_GT,    /* 15 */ &&L_GE,   /* 16 */ &&L_LT,    /* 17 */ &&L_LE,
    /* 18 */ &&L_EQ,    /* 19 */ &&L_AND,  /* 1a */ &&L_OR,    /* 1b */ &&L_NOT,
    /* 1c */ &&L_CONST, /* 1d */ &&L_NOP,  /* 1e */ &&L_IOUT,  /* 1f */ &&L_IIN,

    /* 20 */ &&L_GOTO,  /* 21 */ &&L_BACK, /* 22 */ &&L_NOP,   /* 23 */ &&L_NOP,
    /* 24 */ &&L_IFGT,  /* 25 */ &&L_IFGE, /* 26 */ &&L_IFLT,  /* 27 */ &&L_IFLE,
    /* 28 */ &&L_IFEQ,  /* 29 */ &&L_NEW,  /* 2a */ &&L_SET,   /* 2b */ &&L_GET,
    /* 2c */ &&L_CALL,  /* 2d */ &&L_RET,  /* 2e */ &&L_NOP,   /* 2f */ &&L_NOP,
  };
  INIT_DISPATCH {
	CASE(NOP) {
    } NEXT;
    CASE(EXIT) {
      fout << "  return;" << endl;
    } NEXT;
    CASE(DEBUG) {
    } NEXT;
    CASE(STORE) {
      fout << "  base_pointer[i.operand0] = r" << +i.operand1 << ";" << endl;
    } NEXT;
    CASE(LOAD) {
      fout << "  r" << +i.operand0 << " = base_pointer" << +i.operand1 << ";" << endl;
    } NEXT;
    CASE(PUSH) {
      fout << "  push(r" << +i.operand0 << ");" << endl;
    } NEXT;
    CASE(POP) {
      fout << "  r" << +i.operand0 << " = pop();" << endl;
    } NEXT;
    CASE(COPY) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << ";" << endl;
    } NEXT;
    CASE(SOUT) {
      fout << "  printf(\"" << reinterpret_cast<char*>(&value_placeholder[index_placeholder[strings + i.operand0]]) << "\");" << endl;
    } NEXT;
    CASE(CASL) {
      fout << "  __sync_bool_compare_and_swap(&r" << +i.operand0 << ", r" << +i.operand1 << ", r[i.operand2]);" << endl;
    } NEXT;
    CASE(ADDL) {
      fout << "  __sync_fetch_and_add(&r" << +i.operand0 << ", r" << +i.operand1 << ");" << endl;
    } NEXT;
    CASE(SUBL) {
      fout << "  __sync_fetch_and_sub(&r" << +i.operand0 << ", r" << +i.operand1 << ");" << endl;
    } NEXT;
    CASE(ANDL) {
      fout << "  __sync_fetch_and_and(&r" << +i.operand0 << ", r" << +i.operand1 << ");" << endl;
    } NEXT;
    CASE(ORL) {
      fout << "  __sync_fetch_and_or(&r" << +i.operand0 << ", r" << +i.operand1 << ");" << endl;
    } NEXT;
    CASE(ADD) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " + r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(SUB) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " - r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(MUL) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " * r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(DIV) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " / r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(GT) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " > r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(GE) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " >= r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(LT) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " < r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(LE) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " <= r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(EQ) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " == r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(AND) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " && r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(OR) {
      fout << "  r" << +i.operand0 << " = r" << +i.operand1 << " || r" << +i.operand2 << ";" << endl;
    } NEXT;
    CASE(NOT) {
      fout << "  r" << +i.operand0 << " = !r" << +i.operand1 << ";" << endl;
    } NEXT;
    CASE(CONST) {
      fout << "  __asm volatile(" << endl
           << "    \"movq $"  << +i.operand1 << ", %0\\n\"" << endl
           << "    : \"=r\"(r" << +i.operand0 << ")" << endl
           << "  );" << endl;
    } NEXT;
    CASE(IOUT) {
      fout << "  printf(\"%ld\", r" << +i.operand0 << ");" << endl;
    } NEXT;
    CASE(IIN) {
      fout << "  cin >> r" << +i.operand0 << ";" << endl;
    } NEXT;
    CASE(GOTO) {
      // TODO pc += i.operand0;
    } NEXT;
    CASE(BACK) {
      // TODO pc -= i.operand0;
    } NEXT;
    CASE(IFGT) {
      // TODO if (registers[i.operand1] > registers[i.operand2]) pc += i.operand0;
      // TOOD else pc++;
    } NEXT;
    CASE(IFGE) {
      // TODO if (registers[i.operand1] >= registers[i.operand2]) pc += i.operand0;
      // TODO else pc++;
    } NEXT;
    CASE(IFLT) {
      // TODO if (registers[i.operand1] < registers[i.operand2]) pc += i.operand0;
      // TODO else pc++;
    } NEXT;
    CASE(IFLE) {
      // TODO if (registers[i.operand1] <= registers[i.operand2]) pc += i.operand0;
      // TODO else pc++;
    } NEXT;
    CASE(IFEQ) {
      // TODO if (registers[i.operand1] == registers[i.operand2]) pc += i.operand0;
      // TODO else pc++;
    } NEXT;
    CASE(NEW) {
      // TODO heap.push_back(registers[1]); // TODO back() can be available only if the vector isn't empty.
      // TODO auto tmp = &heap.back();
      // TODO for (uint32_t u = 0; u < i.operand1; u++) heap.push_back(0u);
      // TODO // TODO registers[i.operand0] = (uint32_t) ++tmp;
    } NEXT;
    CASE(SET) {
      // TODO ((uint32_t*) registers[i.operand0])[i.operand1] = registers[i.operand2];
    } NEXT;
    CASE(GET) {
      // TODO registers[i.operand0] = ((uint32_t*) registers[i.operand1])[i.operand2];
    } NEXT;
    CASE(CALL) {
      // TODO
    } NEXT;
    CASE(RET) {
      // TODO
    } NEXT;
  } END_DISPATCH;
#undef INIT_DISPATCH
#undef CASE
#undef NEXT
#undef JUMP
#undef END_DISPATCH

  fout << "}" << endl;

  // assemble
  auto tmps = string("clang -shared -fPIC tmp/");
  tmps += attribute->name; // TODO absolute path
  tmps += ".c -o tmp/";
  tmps += attribute->name;
  tmps += ".so";
  system(tmps.c_str());

  // open child
  auto child_path = string("tmp/");
  child_path += attribute->name;
  child_path += ".so";
  auto child_handle = dlopen(child_path.c_str(), RTLD_LAZY);
  if (attribute->parents != nullptr) {
  // import parents
  for (uint32_t u = 0u; attribute->parents[u] != nullptr; u++) {
    auto parent = attribute->parents[u];
    // open parent
    auto parent_path = string("tmp/");
    parent_path += parent->name;
    parent_path += ".so";
    auto parent_handle = dlopen(parent_path.c_str(), RTLD_LAZY);
    import_function(parent_handle, child_handle, parent->name);
  }
  }
  jit_functions.push_back(reinterpret_cast<void (*)()>(dlsym(child_handle, attribute->name.c_str())));
  return jit_function_count++;
}
void VirtualMachine::jit_execute(uint32_t index) {
  jit_functions[index]();
}
void VirtualMachine::import_function(void* from, void* to, string const& target) {
  void* src = dlsym(from, target.c_str());
  void* dst = dlsym(to, target.c_str());
  *reinterpret_cast<void**>(dst) = src;
}