#ifndef RCWT_VIRTUALMACHINE_H
#define RCWT_VIRTUALMACHINE_H

#include "rcwt.h"

class VirtualMachine {
public:
  VirtualMachine() {
    this->stack = vector<uint32_t>(); stack.reserve(32);
    this->heap = vector<uint32_t>(); heap.reserve(32);
    this->registers = vector<uint32_t>(); registers.reserve(32);
    this->vm_functions = 0u;
    this->jit_functions = vector<void(*)()>();
    this->strings = 0u;
    this->stack_pointer = 0u;
    this->base_pointer = 0u;
    // debugger
    this->vm_function_count = 0u;
    this->vm_function_attributes = vector<vm_function_attribute>();
    this->string_count = 0u;
    // converter.cpp
    this->index_placeholder = vector<uint32_t>(); index_placeholder.reserve(32);
    this->value_placeholder = vector<uint8_t>(); value_placeholder.reserve(32);
    this->value_index = 0u;
    this->index_index = 0u;
	// ll.cpp
	symbols = std::vector<symbol*>(); symbols.reserve(32);
    relocations = std::vector<relocation*>(); relocations.reserve(32);
  }
  void execute();
  uint32_t from_WC(string const&, bool);
  // jit_compiler.cpp
  uint32_t jit_compile(vm_function_attribute*);
  void jit_execute(uint32_t);
  // ll.cpp
  void from_LL(string const&);

private:
  uint32_t entry_point;
  vector<uint32_t> stack;
  vector<uint32_t> heap;
  vector<uint32_t> registers;
  uint32_t vm_functions;
  vector<void (*)()> jit_functions;
  uint32_t strings;
  uint32_t stack_pointer;
  uint32_t base_pointer;
  void push(uint32_t);
  uint32_t pop();
  // debugger
  uint32_t vm_function_count;
  uint32_t jit_function_count;
  vector<vm_function_attribute> vm_function_attributes;
  uint32_t string_count;
  // converter.cpp
  vector<uint32_t> index_placeholder;
  vector<uint8_t> value_placeholder;
  uint32_t value_index;
  uint32_t index_index;
  uint32_t read(uint32_t);
  uint32_t read_int();
  uint32_t read_vm_function();
  uint32_t read_vm_functions();
  uint32_t read_string();
  uint32_t read_strings();
  // jit_compiler.cpp
  void import_function(void*, void*, string const&); // unsafe
  // ll.cpp
  std::vector<symbol*> symbols;
  std::vector<relocation*> relocations;
  void initialize();
  void load(string const&);
  void setup();
  void link();
  void relocate(uint32_t, uint32_t, uint32_t);
  symbol* find_symbol(string const&);
};

#endif