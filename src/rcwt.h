#ifndef RCWT_RCWT_H
#define RCWT_RCWT_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

#define ON_DEBUG

using namespace std;

// main.cpp
void environment();
void dump(void*, uint32_t);
// error.cpp
void error(string const&);
void file_not_found(string const&);
void file_invalid(string const&);
void symbol_not_found(string const&);

struct instruction {
  uint8_t type;
  uint8_t operand0;
  uint8_t operand1;
  uint8_t operand2;
};
struct vm_function_attribute {
  string type;
  string name;
  uint32_t size;
  instruction* body;
  vm_function_attribute** parents;
};
struct symbol {
  string symbol_name;
  bool defined;
// defined == true
  uint32_t base_address;
// defined == false
  string module_name;
};
struct relocation {
  uint32_t base_address;
  string symbol_name;
};

#endif