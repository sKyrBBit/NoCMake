#ifndef RCWT_RCWT_H
#define RCWT_RCWT_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

#define ON_DEBUG

using namespace std;

void environment();
void dump(void*, uint32_t);

#if defined(__LITTLE_ENDIAN__)
struct instruction {
  uint8_t operand2;
  uint8_t operand1;
  uint8_t operand0;
  uint8_t type;
};
#elif defined(__BIG_ENDIAN__)
struct instruction {
  uint8_t type;
  uint8_t operand0;
  uint8_t operand1;
  uint8_t operand2;
};
# endif
struct vm_function_attribute {
  string type;
  string name;
  uint32_t size;
  instruction* body;
  vm_function_attribute** parents;
};

#endif
