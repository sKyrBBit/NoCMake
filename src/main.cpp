#include "rcwt.h"
#include "VirtualMachine.h"
#include <chrono>

using namespace std;

void environment() {
#if defined(__LITTLE_ENDIAN__)
  cout << "log | Endian: Little" << endl;
#elif defined(__BIG_ENDIAN__)
  cout << "log | Endian: Big" << endl;
#else
  int e = 1;
  if (*reinterpret_cast<char*>(&e)) cout << "log | Endian: Little" << endl;
  else cout << "log | Endian: Big" << endl;
#endif
#if defined(_WIN32)
  cout << "log | OS: Windows(32bit)" <<  endl;
#elif defined(__linux)
  cout << "log | OS: Linux" << endl;
#endif
}
void dump(void* target, uint32_t size) {
  char* text = new char[20];
  auto start = reinterpret_cast<unsigned char*>(target);
  auto end = start;
  printf(" 0 1 2 3  4 5 6 7  8 9 A B  C D E F   -- ASCII --\n");
  printf("--------+--------+--------+--------+----------------\n");
  uint32_t i;
  do {
    for (i=0; i<16; i++) {
      text[i] = (0x20u<=*end && *end<=0x7fu) ? *end : '.';
      printf(i%4 == 3 ? "%02X " : "%02X", *end);
      end++;
      if (end-start>=size) {
        text[++i] = '\0';
        for (; i<16; i++) printf(i%4 == 3 ? "   " : "  ");
      }
    }
    printf("%s\n", text);
  } while(end-start<size);
}


uint32_t gen(string const& type, uint8_t operand0, uint8_t operand1, uint8_t operand2) {
  uint8_t tmp;
  if (type == "nop" || type == "NOP")     tmp = 0x0u;
  if (type == "exit" || type == "EXIT")   tmp = 0x1u;
  if (type == "debug" || type == "DEBUG") tmp = 0x2u;
  if (type == "store" || type == "STORE") tmp = 0x4u;
  if (type == "load" || type == "LOAD")   tmp = 0x5u;
  if (type == "push" || type == "PUSH")   tmp = 0x6u;
  if (type == "pop" || type == "POP")     tmp = 0x7u;
  if (type == "copy" || type == "COPY")   tmp = 0x8u;
  if (type == "sout" || type == "SOUT")   tmp = 0xau;
  if (type == "casl" || type == "CASL")   tmp = 0xbu;
  if (type == "addl" || type == "ADDL")   tmp = 0xcu;
  if (type == "subl" || type == "SUBL")   tmp = 0xdu;
  if (type == "andl" || type == "ANDL")   tmp = 0xeu;
  if (type == "orl" || type == "ORL")     tmp = 0xfu;
  if (type == "add" || type == "ADD")     tmp = 0x10u;
  if (type == "sub" || type == "SUB")     tmp = 0x11u;
  if (type == "mul" || type == "MUL")     tmp = 0x12u;
  if (type == "div" || type == "DIV")     tmp = 0x13u;
  if (type == "gt" || type == "GT")       tmp = 0x14u;
  if (type == "ge" || type == "GE")       tmp = 0x15u;
  if (type == "lt" || type == "LT")       tmp = 0x16u;
  if (type == "le" || type == "LE")       tmp = 0x17u;
  if (type == "eq" || type == "EQ")       tmp = 0x18u;
  if (type == "and" || type == "AND")     tmp = 0x19u;
  if (type == "or" || type == "OR")       tmp = 0x1au;
  if (type == "not" || type == "NOT")     tmp = 0x1bu;
  if (type == "const" || type == "CONST") tmp = 0x1cu;
  if (type == "iout" || type == "IOUT")   tmp = 0x1eu;
  if (type == "iin" || type == "IIN")     tmp = 0x1fu;
  if (type == "goto" || type == "GOTO")   tmp = 0x20u;
  if (type == "back" || type == "BACK")   tmp = 0x21u;
  if (type == "ifgt" || type == "IFGT")   tmp = 0x24u;
  if (type == "ifge" || type == "FIGE")   tmp = 0x25u;
  if (type == "iflt" || type == "IFLT")   tmp = 0x26u;
  if (type == "ifle" || type == "IFLE")   tmp = 0x27u;
  if (type == "ifeq" || type == "IFEQ")   tmp = 0x28u;
  if (type == "new" || type == "NEW")     tmp = 0x29u;
  if (type == "set" || type == "SET")     tmp = 0x2au;
  if (type == "get" || type == "GET")     tmp = 0x2bu;
  if (type == "call" || type == "CALL")   tmp = 0x2cu;
  if (type == "ret" || type == "RET")     tmp = 0x2du;
  return (tmp << 24) + (operand0 << 16) + (operand1 << 8) + operand2;
}

uint32_t ti;
#define write_int(i) ti = i;fout.write(reinterpret_cast<char*>(&ti), 4)
#define write_str(s) fout << s
#define write_ins(s, i, j, k) write_int(gen(s, i, j, k))

void test1() {
  auto vm = new VirtualMachine;
  // write test1
  ofstream fout("tmp/test1.wc", ios::binary);
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
  // read test1
  auto main1 = vm->from_WC("tmp/test1", true);
  // execute test1
  auto start1 = chrono::system_clock::now();
  vm->execute(main1);
  auto end1 = chrono::system_clock::now();
  auto elapsed1 = chrono::duration_cast<chrono::milliseconds>(end1-start1).count();
  printf("\ntime %ld[ms]\n", elapsed1);
  // // jit_compile main1
  // auto attribute = new vm_function_attribute;
  // attribute->type = "void";
  // attribute->name = "test1";
  // attribute->size = 17;
  // attribute->body = main1;
  // auto index1 = vm->jit_compile(attribute);
  // vm->jit_execute(index1);
}

void test2() {
  auto vm = new VirtualMachine;
  // write test2
  ofstream fout("tmp/test2.wc", ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x00000002u); // function_size: 2
  write_int(gen("sout", 0, 0, 0)); // out s0
  write_int(gen("exit", 0, 0, 0)); // exit
  write_int(0x00000000u); // method_count: 0
  write_int(0x00000001u); // string_count: 1
  write_int(0x0000000du); // string_size: 13
  write_str("Hello World!");
  fout.close();
  // read test2
  auto main2 = vm->from_WC("tmp/test2", true);
  // execute test2
  auto start2 = chrono::system_clock::now();
  vm->execute(main2);
  auto end2 = chrono::system_clock::now();
  auto elapsed2 = chrono::duration_cast<chrono::milliseconds>(end2-start2).count();
  printf("\ntime %ld[ms]\n", elapsed2);
  // // jit_compile main2
  // auto attribute = new vm_function_attribute;
  // attribute->type = "void";
  // attribute->name = "test2";
  // attribute->size = 2;
  // attribute->body = main2;
  // auto index2 = vm->jit_compile(attribute);
  // vm->jit_execute(index2);
}

void test3() {
  auto vm = new VirtualMachine;
  // write test3
  ofstream fout("tmp/test3.wc", ios::binary);
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
  // read test3
  auto main3 = vm->from_WC("tmp/test3", true);
  // execute test3
  auto start3 = chrono::system_clock::now();
  vm->execute(main3);
  auto end3 = chrono::system_clock::now();
  auto elapsed3 = chrono::duration_cast<chrono::milliseconds>(end3-start3).count();
  printf("\ntime %ld[ms]\n", elapsed3);
}
void test4() {
  auto vm = new VirtualMachine;
  // write test4a
  ofstream fout("tmp/test4a.wc", ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x00000002u); // function_size: 2
  write_ins("call", 0, 0, 0); // call 0 0 0
  write_ins("exit", 0, 0, 0); // exit
  write_int(0x00000000u); // method_count: 0
  write_int(0x00000001u); // string_count: 1
  write_int(0x00000006u); // string_size: 6
  write_str("test4");
  fout.close();
  // write test4b
  fout.open("tmp/test4b.wc", ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x00000000u); // function_size: 0
  write_int(0x00000001u); // method_count: 1
  write_int(0x00000002u); // function_size: 2
  write_ins("sout", 0, 0, 0); // out s0
  write_ins("exit", 0, 0, 0); // exit
  write_int(0x00000000u); // string_count: 0
  fout.close();
  // read test4a
  auto main4a = vm->from_WC("tmp/test4a", true);
  vm->from_WC("tmp/test4b", false);
  // execute test4a
  auto start4 = chrono::system_clock::now();
  vm->execute(main4a);
  auto end4 = chrono::system_clock::now();
  auto elapsed4 = chrono::duration_cast<chrono::milliseconds>(end4-start4).count();
  printf("\ntime %ld[ms]\n", elapsed4);
}

int main() {
  test1();
  cout << endl;
  test2();
  cout << endl;
  test3();
  cout << endl;
  return 0;
}

#undef write_int
#undef write_str
#undef write_ins