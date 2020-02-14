#ifndef RCWT_VIRTUALMACHINE_H
#define RCWT_VIRTUALMACHINE_H

#include "rcwt.h"

class VirtualMachine {
public:
  VirtualMachine() {
    this->stack = vector<uint32_t>(1);
    this->heap = vector<uint32_t>(1);
    this->registers = vector<uint32_t>(32);
    this->vm_functions = nullptr;
    this->jit_functions = vector<void(*)()>();
    this->strings = nullptr;
    this->stack_pointer = 0;
    this->base_pointer = 0;
    // debug
    this->vm_function_count = 0u;
    this->vm_function_attributes = vector<vm_function_attribute>();
    this->string_count = 0u;
    // converter
    this->pointer_placeholder = vector<uint32_t*>();
    this->value_placeholder = vector<uint8_t>();
    this->index = 0u;
  }
  void execute(instruction*);
  instruction* from_WC(string const&);
  // JIT compiler
  uint32_t jit_compile(vm_function_attribute*);
  void jit_execute(uint32_t);

private:
  vector<uint32_t> stack;
  vector<uint32_t> heap;
  vector<uint32_t> registers;
  instruction** vm_functions;
  vector<void (*)()> jit_functions;
  char** strings;
  uint32_t stack_pointer;
  uint32_t base_pointer;
  void push(uint32_t);
  uint32_t pop();
  // debuger
  uint32_t vm_function_count;
  uint32_t jit_function_count;
  vector<vm_function_attribute> vm_function_attributes;
  uint32_t string_count;
  // converter
  vector<uint32_t*> pointer_placeholder;
  vector<uint8_t> value_placeholder;
  uint32_t index;
  uint8_t* read(uint32_t);
  uint32_t read_int();
  instruction* read_vm_function();
  instruction** read_vm_functions();
  char* read_string();
  char** read_strings();
  // JIT compiler
  void import_function(void*, void*, string const&); // unsafe
};

#endif
