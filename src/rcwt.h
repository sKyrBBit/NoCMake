#ifndef RCWT_RCWT_H
#define RCWT_RCWT_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

using namespace std;

struct instruction {
  uint8_t operand2;
  uint8_t operand1;
  uint8_t operand0;
  uint8_t type;
};
struct vm_function_attribute {
  string type;
  string name;
  uint32_t size;
  instruction* body;
  vm_function_attribute** parents;
};

#endif
