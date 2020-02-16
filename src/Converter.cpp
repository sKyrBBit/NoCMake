#include "VirtualMachine.h"
#ifdef ON_DEBUG

#define DEBUG_OUT(message, target)     cout << "debug | " << message << target << endl
#define DEBUG_OUT_HEX(message, target) cout << "debug | " << message << hex << target << endl

#else

#define DEBUG_OUT(message, target)
#define DEBUG_OUT_HEX(message, target)

#endif
inline uint32_t VirtualMachine::read(uint32_t size) {
  auto tmp = value_index;
  value_index += size;
  return tmp;
}
inline uint32_t VirtualMachine::read_int() {
  auto tmp = &value_placeholder[value_index];
  value_index += 4;
  return *reinterpret_cast<uint32_t*>(tmp);
}
inline uint32_t VirtualMachine::read_vm_function() {
  auto vm_function_size = read_int();
  DEBUG_OUT_HEX("vm_function size : ", vm_function_size);
  if (vm_function_size == 0u) return 0;
  // auto attribute = new vm_function_attribute;
  // attribute->type = "void";
  // attribute->name = read_string();
  // attribute->size = 0;
  // attribute->body = nullptr;
  // attribute->parents = nullptr;
  return read(vm_function_size * 4);
}
inline uint32_t VirtualMachine::read_string() {
  auto string_size = read_int();
  DEBUG_OUT_HEX("string size : ", string_size);
  if (string_size == 0u) return 0;
  return read(string_size);
}
inline uint32_t VirtualMachine::read_vm_functions() {
  vm_function_count = read_int();
  DEBUG_OUT_HEX("vm_function count : ", vm_function_count);
  if (vm_function_count == 0) return 0;
  auto tmp = index_index;
  for (uint32_t i = 0u; i < vm_function_count; i++) {
    index_placeholder.push_back(read_vm_function());
    index_index++;
  }
  return tmp;
}
inline uint32_t VirtualMachine::read_strings() {
  string_count = read_int();
  DEBUG_OUT_HEX("string count : ", string_count);
  if (string_count == 0u) return 0;
  auto tmp = index_index;
  for (uint32_t i = 0u; i < string_count; i++) {
    index_placeholder.push_back(read_string());
    index_index++;
  }
  return tmp;
}
uint32_t VirtualMachine::from_WC(string const& file_name, bool is_main) {
  // read `wc` file
  string wc = string(file_name);
  wc += ".wc";
  ifstream fin(wc.c_str(), ios::binary);
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
    return 0;
  }
  while (!fin.eof()) {
    uint8_t buffer = 0;
    fin.read(reinterpret_cast<char*>(&buffer), 1);
    value_placeholder.push_back(buffer);
  }
  fin.close();

  // load wc into memory
  if (is_main) {
    auto main = read_vm_function();
    vm_functions = read_vm_functions();
    strings = read_strings();
    return main;
  } else {
    read_vm_function();
    read_vm_functions();
    read_strings();
    return 0;
  }
}

#undef DEBUG_OUT
#undef DEBUG_OUT_HEX