#include <cstdint>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

using namespace std;

bool is_little_endian() {
#if defined(__LITTLE_ENDIAN__)
  return true;
#elif defined(__BIG_ENDIAN__)
  return false;
#else
  int e = 1;
  if (*reinterpret_cast<char*>(&e)) return true;
  else return false;
#endif
}


uint32_t gen(string const& type, uint8_t operand0, uint8_t operand1, uint8_t operand2) {
  unordered_map<string, uint8_t> str2ins = {
    {"nop",   0x00u}, {"NOP",   0x00u},
    {"exit",  0x01u}, {"EXIT",  0x01u},
    {"debug", 0x02u}, {"DEBUG", 0x02u},
    {"store", 0x04u}, {"STORE", 0x04u},
    {"load",  0x05u}, {"LOAD",  0x05u},
    {"push",  0x06u}, {"PUSH",  0x06u},
    {"pop",   0x07u}, {"POP",   0x07u},
    {"copy",  0x08u}, {"COPY",  0x08u},
    {"sout",  0x0au}, {"SOUT",  0x0au},
    {"casl",  0x0bu}, {"CASL",  0x0bu},
    {"addl",  0x0cu}, {"ADDL",  0x0cu},
    {"subl",  0x0du}, {"SUBL",  0x0du},
    {"andl",  0x0eu}, {"ANDL",  0x0eu},
    {"orl",   0x0fu}, {"ORL",   0x0fu},
    {"addr",  0x10u}, {"ADDR",  0x10u},
    {"subr",  0x11u}, {"SUBR",  0x11u},
    {"mulr",  0x12u}, {"MULR",  0x12u},
    {"divr",  0x13u}, {"DIVR",  0x13u},
    {"addi",  0x14u}, {"ADDI",  0x14u},
    {"subi",  0x15u}, {"SUBI",  0x15u},
    {"muli",  0x16u}, {"MULI",  0x16u},
    {"divi",  0x17u}, {"DIVI",  0x17u},
    {"gt",    0x18u}, {"GT",    0x18u},
    {"ge",    0x19u}, {"GE",    0x19u},
    {"eq",    0x1au}, {"EQ",    0x1au},
    {"const", 0x1bu}, {"CONST", 0x1bu},
    {"and",   0x1cu}, {"AND",   0x1cu},
    {"or",    0x1du}, {"OR",    0x1du},
    {"not",   0x1eu}, {"NOT",   0x1eu},
    {"goto",  0x20u}, {"GOTO",  0x20u},
    {"back",  0x21u}, {"BACK",  0x21u},
    {"ifgt",  0x24u}, {"IFGT",  0x24u},
    {"ifge",  0x25u}, {"FIGE",  0x25u},
    {"ifeq",  0x26u}, {"IFEQ",  0x26u},
    {"iout",  0x27u}, {"IOUT",  0x27u},
    {"iin",   0x28u}, {"IIN",   0x28u},
    {"new",   0x29u}, {"NEW",   0x29u},
    {"set",   0x2au}, {"SET",   0x2au},
    {"get",   0x2bu}, {"GET",   0x2bu},
    {"call",  0x2cu}, {"CALL",  0x2cu},
    {"ret",   0x2du}, {"RET",   0x2du}
  };
  if (is_little_endian()) return str2ins[type] + (operand0 << 8) + (operand1 << 16) + (operand2 << 24);
  else return (str2ins[type] << 24) + (operand0 << 16) + (operand1 << 8) + operand2;
}

uint32_t ti;
#define write_int(i) ti = i;fout.write(reinterpret_cast<char*>(&ti), 4)
#define write_str(s) fout << s
#define write_ins(s, i, j, k) write_int(gen(s, i, j, k))

void test1() {
  // write test1
  ofstream fout("tmp/_test1.wc", ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x00000011u); // function_size: 17
  write_ins("const", 2, 1, 0);   // r2 = 1
  write_ins("const", 3, 255, 0); // r3 = 255
  write_ins("const", 0, 0, 0);   // r0 = 0
  write_ins("ifgt", 13, 0, 3);   // if (r0 > r3) goto +13
  write_ins("const", 1, 0, 0);   // r1 = 0
  write_ins("ifgt", 9, 1, 3);    // if (r1 > r3) goto +9
  write_ins("const", 4, 1, 0);   // r4 = 1
  write_ins("const", 5, 2, 0);   // r5 = 2
  write_ins("const", 6, 3, 0);   // r6 = 3
  write_ins("mul", 7, 5, 6);     // r7 = r5 * r6
  write_ins("add", 8, 4, 7);     // r8 = r4 + r7
  write_ins("iout", 8, 0, 0);    // out r8
  write_ins("add", 1, 1, 2);     // r1 = r1 + r2
  write_ins("back", 8, 0, 0);    // goto -8
  write_ins("add", 0, 0, 2);     // r0 = r0 + r2
  write_ins("back", 12, 0, 0);   // goto -12
  write_ins("exit", 0, 0, 0);    // exit
  write_int(0x00000000u); // method_count: 0
  write_int(0x00000000u); // string_count: 0
  fout.close();
}

void test2() {
  // write test2
  ofstream fout("tmp/_test2.wc", ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x00000002u); // function_size: 2
  write_int(gen("sout", 0, 0, 0)); // out s0
  write_int(gen("exit", 0, 0, 0)); // exit
  write_int(0x00000000u); // method_count: 0
  write_int(0x00000001u); // string_count: 1
  write_int(0x0000000du); // string_size: 13
  write_str("Hello World!");
  fout.close();
}

void test3() {
  // write test3
  ofstream fout("tmp/_test3.wc", ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x00000002u); // function_size: 2
  write_ins("call", 0, 0, 1);   // call 0 0 1
  write_ins("exit", 0, 0, 0);   // exit
  write_int(0x00000001u); // method_count: 1
  write_int(0x00000005u); // function_size: 5
  write_ins("const", 0, 0, 0);  // r0 = 0
  write_ins("store", 0, 0, 0);  // l0 = r0
  write_ins("load", 1, 0, 0);   // r1 = l0
  write_ins("iout", 1, 0, 0);   // iout r1
  write_ins("ret", 0, 0, 0);    // ret
  write_int(0x00000000u); // string_count: 0
  fout.close();
}
void test4() {
  // write test4a
  ofstream fout("tmp/_test4a.ll", ios::binary);
  write_int(0x52435754u);    // magic
  write_int(0x00000002u);    // symbol_count
  write_int(0x00000001u);    // relocation_count
  write_int(0x00000008u);    // data_size
  write_int(0x00000004u);
  write_str("main");         // symbol_name
  write_int(0x00000001u);    // defined
  write_int(0x00000000u);    // base_address
  write_int(0x00000002u);
  write_str("f0");           // symbol_name
  write_int(0x00000000u);    // defined
  write_int(0x0000000bu);
  write_str("tmp/_test4b");  // module_name
  write_int(0x00000002u);    // base_address
  write_int(0x00000002u);
  write_str("f0");           // symbol_name
  write_ins("call", 0, 0, 0); // call 0 0 0
  write_ins("exit", 0, 0, 0); // exit
  fout.close();
  // write test4b
  fout.open("tmp/_test4b.ll", ios::binary);
  write_int(0x52435754u);    // magic
  write_int(0x00000002u);    // symbol_count
  write_int(0x00000001u);    // relocation_count
  write_int(0x00000016u);    // data_size
  write_int(0x00000002u);
  write_str("f0");           // symbol_name
  write_int(0x00000001u);    // defined
  write_int(0x00000000u);    // base_address
  write_int(0x00000002u);
  write_str("s0");           // symbol_name
  write_int(0x00000001u);    // defined
  write_int(0x00000008u);    // base_address
  write_int(0x00000001u);    // base_address
  write_int(0x00000002u);
  write_str("s0");           // symbol_name
  write_ins("sout", 8, 0, 0); // out s0
  write_ins("exit", 0, 0, 0); // exit
  write_str("Hello, World!");
  write_int(0x00000000u);
  fout.close();
}

void test5() {
  ofstream fout("tmp/_test5.ll", ios::binary);
  write_int(0x52435754u);    // magic
  write_int(0x00000002u);    // symbol_count
  write_int(0x00000000u);    // relocation_count
  write_int(0x0000000cu);    // data_size
  write_int(0x00000004u);
  write_str("main");         // symbol_name
  write_int(0x00000001u);    // defined
  write_int(0x00000000u);    // base_address
  write_int(0x00000002u);
  write_str("s0");           // symbol_name
  write_int(0x00000001u);    // defined
  write_int(0x00000008u);    // base_address
  write_ins("const", 0, 100, 0);
  write_ins("iout", 0, 0, 0);
  write_ins("exit", 0, 0, 0);
  fout.close();
}

#undef write_int
#undef write_str
#undef write_ins

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
}