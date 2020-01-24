#ifndef RCWT_RCWT_H
#define RCWT_RCWT_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

using namespace std;

struct instruction {
  uint8_t type;
  uint8_t operand0;
  uint8_t operand1;
  uint8_t operand2;
};

#endif
