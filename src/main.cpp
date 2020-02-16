#include "rcwt.h"
#include "VirtualMachine.h"
#include <chrono>
#include <unordered_map>

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
    {"add",   0x10u}, {"ADD",   0x10u},
    {"sub",   0x11u}, {"SUB",   0x11u},
    {"mul",   0x12u}, {"MUL",   0x12u},
    {"div",   0x13u}, {"DIV",   0x13u},
    {"gt",    0x14u}, {"GT",    0x14u},
    {"ge",    0x15u}, {"GE",    0x15u},
    {"lt",    0x16u}, {"LT",    0x16u},
    {"le",    0x17u}, {"LE",    0x17u},
    {"eq",    0x18u}, {"EQ",    0x18u},
    {"and",   0x19u}, {"AND",   0x19u},
    {"or",    0x1au}, {"OR",    0x1au},
    {"not",   0x1bu}, {"NOT",   0x1bu},
    {"const", 0x1cu}, {"CONST", 0x1cu},
    {"iout",  0x1eu}, {"IOUT",  0x1eu},
    {"iin",   0x1fu}, {"IIN",   0x1fu},
    {"goto",  0x20u}, {"GOTO",  0x20u},
    {"back",  0x21u}, {"BACK",  0x21u},
    {"ifgt",  0x24u}, {"IFGT",  0x24u},
    {"ifge",  0x25u}, {"FIGE",  0x25u},
    {"iflt",  0x26u}, {"IFLT",  0x26u},
    {"ifle",  0x27u}, {"IFLE",  0x27u},
    {"ifeq",  0x28u}, {"IFEQ",  0x28u},
    {"new",   0x29u}, {"NEW",   0x29u},
    {"set",   0x2au}, {"SET",   0x2au},
    {"get",   0x2bu}, {"GET",   0x2bu},
    {"call",  0x2cu}, {"CALL",  0x2cu},
    {"ret",   0x2du}, {"RET",   0x2du}
  };
  return (str2ins[type] << 24) + (operand0 << 16) + (operand1 << 8) + operand2;
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