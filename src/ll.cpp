#include "VirtualMachine.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>

void VirtualMachine::initialize() {
#define FILL_NULL(target) fill(target.begin(), target.end(), nullptr)
#define FILL_ZERO(target) fill(target.begin(), target.end(), 0)

  FILL_NULL(symbols);
  FILL_NULL(relocations);
  FILL_ZERO(value_placeholder);

#undef FILL_NULL
#undef FILL_ZERO
}
void VirtualMachine::from_LL(string const& target) {
  initialize();
  load(target);
  setup();
}
void VirtualMachine::load(string const& target) { // TODO simplify
  ifstream fin(target + ".ll", ios::binary);
  if (!fin) file_not_found(target);

#ifdef ON_DEBUG

#define DEBUG_OUT_HEX(message, target) cout << "debug | " << message << hex << target << endl
#define DEBUG_OUT_STR(message, target) cout << "debug | " << message << target << endl

#else

#define DEBUG_OUT HEX

#endif

#define READ_INT(target) fin.read(reinterpret_cast<char*>(&target), 4); DEBUG_OUT_HEX(#target + string(": "), target)

  uint32_t magic;
  READ_INT(magic);
  if (magic != 0x52435754) { // "RCWT"
    file_invalid(target + ".ll");
    fin.close();
    return;
  }
  // save vector counts for relocation
  auto symbol_begin = symbols.size();
  auto relocation_begin = relocations.size();
  auto offset = value_placeholder.size();

  uint32_t symbol_count;
  READ_INT(symbol_count);
  uint32_t relocation_count;
  READ_INT(relocation_count);
  uint32_t data_size;
  READ_INT(data_size);

  for (uint32_t u = 0; u < symbol_count; u++) {
    auto tmps = new symbol;
    symbols.push_back(tmps);
	uint32_t symbol_name_length;
    READ_INT(symbol_name_length);
	tmps->symbol_name = string();
	for (uint32_t u = 0; u < symbol_name_length; u++) {
	  char tmpc;
      fin.read(reinterpret_cast<char*>(&tmpc), 1);
      tmps->symbol_name += tmpc;
	}
	DEBUG_OUT_STR("symbol name: ", tmps->symbol_name);
    READ_INT(tmps->defined);
    if (tmps->defined) {
      READ_INT(tmps->base_address);
    } else {
      uint32_t module_name_length;
	  READ_INT(module_name_length);
	  tmps->module_name = string();
	  for (uint32_t u = 0; u < module_name_length; u++) {
	    char tmpc;
        fin.read(reinterpret_cast<char*>(&tmpc), 1);
        tmps->module_name += tmpc;
	  }
	  DEBUG_OUT_STR("module name: ", tmps->module_name);
    }
  }

  for (uint32_t u = 0; u < relocation_count; u++) {
    auto tmpr = new relocation;
    relocations.push_back(tmpr);
    READ_INT(tmpr->base_address);
	uint32_t symbol_name_length;
	READ_INT(symbol_name_length);
	tmpr->symbol_name = string();
	for (uint32_t u = 0; u < symbol_name_length; u++) {
	  char tmpc;
      fin.read(reinterpret_cast<char*>(&tmpc), 1);
      tmpr->symbol_name += tmpc;
	}
	DEBUG_OUT_STR("symbol name: ", tmpr->symbol_name);
  }

  for (uint32_t u = 0; u < data_size; u++) {
   	uint8_t tmp;
   	fin.read(reinterpret_cast<char*>(&tmp), 1);
   	value_placeholder.push_back(tmp);
  }
  if (relocation_count > 0) { // TODO
	relocate(symbol_begin, relocation_begin, offset);
	link();
  }

#undef DEBUG_OUT_HEX
#undef READ_INT 
}
void VirtualMachine::setup() {
  for (symbol* s : symbols) {
	if (s->symbol_name == "main") {
	  entry_point = s->base_address;
	  break;
	}
  }
  // TODO? symbols -> index_placeholder
}
void VirtualMachine::link() { // TODO check
  for (relocation* tmpr : relocations) {
    auto tmps = find_symbol(tmpr->symbol_name);
    if (!tmps->defined) {
	  // TODO remove
	  tmps->symbol_name = ""; tmps->defined = true;
  	  load(tmps->module_name);
	  // resolve
	  value_placeholder[tmpr->base_address] = find_symbol(tmpr->symbol_name)->base_address;
    } else {
      // resolve
	  value_placeholder[tmpr->base_address] = tmps->base_address;
	}
  }
}
void VirtualMachine::relocate(uint32_t symbol_begin, uint32_t relocation_begin, uint32_t offset) {
  for (uint32_t u = symbol_begin; u < symbols.size(); u++) {
	if (symbols[u]->defined) symbols[u]->base_address += offset;
  }
  for (uint32_t u = relocation_begin; u < relocations.size(); u++) {
	relocations[u]->base_address += offset;
  }
}
inline symbol* VirtualMachine::find_symbol(string const& name) {
  for (symbol* s : symbols) if (s->symbol_name == name) return s;
  symbol_not_found(name);
  exit(1);
}