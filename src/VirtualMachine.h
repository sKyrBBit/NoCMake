#ifndef RCWT_VIRTUALMACHINE_H
#define RCWT_VIRTUALMACHINE_H

#include "rcwt.h"
#include <utility>
#include <string>

class VirtualMachine {
public:
  VirtualMachine() {
    this->stack = vector<uint32_t>(32);
    this->heap = vector<uint32_t>(32);
    this->registers = vector<uint32_t>(32);
    this->vm_functions = nullptr;
    this->stack_pointer = &stack.front();
    this->base_pointer = nullptr;
    // converter
    this->value_placeholder = vector<uint32_t>(32);
    this->pointer_placeholder = vector<uint32_t*>(32);
    this->input = vector<uint8_t>(32);
    this->position = 0u;
  }
  void execute(instruction*);
  instruction* from_WC(string const&);

private:
  vector<uint32_t> stack;
  vector<uint32_t> heap;
  vector<uint32_t> registers;
  instruction** vm_functions;
  uint32_t* stack_pointer;
  uint32_t* base_pointer;
  void push(uint32_t);
  uint32_t pop();
  // converter
  vector<uint32_t> value_placeholder;
  vector<uint32_t*> pointer_placeholder;
  vector<uint8_t> input;
  uint32_t position;
  uint8_t* read(uint32_t);
  uint32_t read_int();
  instruction* read_function();
  instruction** read_methods();
};

#endif
