#include "VirtualMachine.h"
#include <utility>
#include <string>
#include <dlfcn.h>

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
void VirtualMachine::execute(instruction* main) {
  // direct threading
#define INIT_DISPATCH JUMP;
#define CASE(op) L_ ## op:
#define NEXT i=*++pc; goto *table[i.type]
#define JUMP i=*pc; goto *table[i.type]
#define END_DISPATCH L_END:
  auto pc = main;
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
      goto L_END;
    } NEXT;
    CASE(DEBUG) {
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
      cout << strings[i.operand0];
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
    CASE(ADD) {
      registers[i.operand0] = registers[i.operand1] + registers[i.operand2];
    } NEXT;
    CASE(SUB) {
      registers[i.operand0] = registers[i.operand1] - registers[i.operand2];
    } NEXT;
    CASE(MUL) {
      registers[i.operand0] = registers[i.operand1] * registers[i.operand2];
    } NEXT;
    CASE(DIV) {
      registers[i.operand0] = registers[i.operand1] / registers[i.operand2];
    } NEXT;
    CASE(GT) {
      registers[i.operand0] = registers[i.operand1] > registers[i.operand2];
    } NEXT;
    CASE(GE) {
      registers[i.operand0] = registers[i.operand1] >= registers[i.operand2];
    } NEXT;
    CASE(LT) {
      registers[i.operand0] = registers[i.operand1] < registers[i.operand2];
    } NEXT;
    CASE(LE) {
      registers[i.operand0] = registers[i.operand1] <= registers[i.operand2];
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
    CASE(IFLT) {
      if (registers[i.operand1] < registers[i.operand2]) pc += i.operand0;
      else pc++;
    } JUMP;
    CASE(IFLE) {
      if (registers[i.operand1] <= registers[i.operand2]) pc += i.operand0;
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
      pc = vm_functions[i.operand1];
    } JUMP;
    CASE(RET) {
      // return address
      pc = reinterpret_cast<instruction*>(pop());
      // locals
      stack_pointer = base_pointer;
      // base pointer
      base_pointer = pop();
    } JUMP;
  } END_DISPATCH;
}
#undef INIT_DISPATCH
#undef CASE
#undef NEXT
#undef JUMP
#undef END_DISPATCH

#define ON_DEBUG

#ifdef ON_DEBUG

#define DEBUG_OUT(message, target)     cout << "debug | " << message << target << endl
#define DEBUG_OUT_HEX(message, target) cout << "debug | " << message << hex << target << endl

#else

#define DEBUG_OUT(message, target)
#define DEBUG_OUT_HEX(message, target)

#endif

inline uint8_t* VirtualMachine::read(uint32_t size) {
  auto tmp = &value_placeholder[index];
  index += size;
  return tmp;
}
inline uint32_t VirtualMachine::read_int() {
  return *reinterpret_cast<uint32_t*>(read(4));
}
inline instruction* VirtualMachine::read_vm_function() {
  auto vm_function_size = read_int();
  DEBUG_OUT_HEX("vm_function size : ", vm_function_size);
  if (vm_function_size == 0u) return nullptr;
  // auto attribute = new vm_function_attribute;
  // attribute->type = "void";
  // attribute->name = read_string();
  // attribute->size = 0;
  // attribute->body = nullptr;
  // attribute->parents = nullptr;
  return reinterpret_cast<instruction*>(read(vm_function_size * 4));
}
inline char* VirtualMachine::read_string() {
  auto string_size = read_int();
  DEBUG_OUT_HEX("string size : ", string_size);
  if (string_size == 0u) return nullptr;
  return reinterpret_cast<char*>(read(string_size));
}
inline instruction** VirtualMachine::read_vm_functions() {
  vm_function_count = read_int();
  DEBUG_OUT_HEX("vm_function count : ", vm_function_count);
  if (vm_function_count == 0) return nullptr;
  auto is_empty = pointer_placeholder.empty(); // TODO back() can be available only if the vector isn't empty.
  auto tmp = &pointer_placeholder.back();
  for (uint32_t i = 0u; i < vm_function_count; i++) {
    pointer_placeholder.push_back(reinterpret_cast<uint32_t*>(read_vm_function()));
  }
  return is_empty
    ? reinterpret_cast<instruction**>(&pointer_placeholder.front())
    : reinterpret_cast<instruction**>(++tmp);
}
inline char** VirtualMachine::read_strings() {
  string_count = read_int();
  DEBUG_OUT_HEX("string count : ", string_count);
  if (string_count == 0u) return nullptr;
  auto is_empty = pointer_placeholder.empty(); // TODO back() can be available only if the vector isn't empty.
  auto tmp = &pointer_placeholder.back();
  for (uint32_t i = 0u; i < string_count; i++) {
    pointer_placeholder.push_back(reinterpret_cast<uint32_t*>(read_string()));
  }
  return is_empty
    ? reinterpret_cast<char**>(&pointer_placeholder.front())
    : reinterpret_cast<char**>(++tmp);
}
instruction* VirtualMachine::from_WC(string const& file_name) {
  // read `wc` file
  value_placeholder.clear(); // TODO remove
  index = 0u;
  string wc = string(file_name);
  wc += ".wc";
  ifstream fin(wc.c_str(), ios::in | ios::binary);
  if (!fin) {
    cout << "error | file `" << wc.c_str() << "` didn't open" << endl;
    exit(1);
  }
  // check if magic is valid or not
  uint32_t magic = 0u;
  fin.read(reinterpret_cast<char*>(&magic), 4);
  DEBUG_OUT_HEX("magic : ", magic);
  if (magic != 0xdeadbeef) {
    cout << "error | file `" << wc.c_str() << "` is invalid" << endl;
    fin.close();
    return nullptr;
  }
  while (!fin.eof()) {
    uint8_t buffer = 0;
    fin.read(reinterpret_cast<char*>(&buffer), 1);
    value_placeholder.push_back(buffer);
  }
  fin.close();
  
  // load wc into memory
  auto main = read_vm_function();
  vm_functions = read_vm_functions();
  strings = read_strings();
  return main;
}

#undef DEBUG_OUT
#undef DEBUG_OUT_HEX

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
      fout << "  printf(\"" << strings[i.operand0] << "\");" << endl;
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