#include "VirtualMachine.h"
#include <string>

inline void error(string const& message) {
  cout << "error | " << message << endl;
}
/* inline */ void file_not_found(string const& file_name) {
  error("FileNotFound: " + file_name);
}
/* inline */ void file_invalid(string const& file_name) {
  error("FileInvalid: " + file_name);
}
/* inline */ void symbol_not_found(string const& symbol_name) {
  error("SymbolNotFound" + symbol_name);
}