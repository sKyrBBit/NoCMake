#include "rcwt.h"
#include "VirtualMachine.h"
#include <chrono>

using namespace std;

void environment() {
#if defined(__LITTLE_ENDIAN__)
  cout << "Endian: Little" << endl;
#elif defined(__BIG_ENDIAN__)
  cout << "Endian: Big" << endl;
#else
  int e = 1;
  if (*reinterpret_cast<char*>(&e)) cout << "Endian: Little" << endl;
  else cout << "Endian: Big" << endl;
#endif
#if defined(_WIN32)
  cout << "OS: Windows(32bit)" <<  endl;
#elif defined(__linux)
  cout << "OS: Linux" << endl;
#endif
}
void dump(const void* buf, uint32_t size) {
    int i,j;
    unsigned char* p = (unsigned char* )buf, tmp[20];

    printf("+0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F|  -- ASCII --\r\n");
    printf("--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+----------------\r\n");
    for (i=0; p-(unsigned char* )buf<size; i++) {
        for (j=0; j<16; j++) {
            tmp[j] = (unsigned char)((*p<0x20||*p>=0x7f)? '.': *p);
            printf("%02X ", (int)*p);
            if (++p-(unsigned char *)buf>=size) {
                tmp[++j] = '\0';
                for (;j<16;j++) {
                    printf("   ");
                }
                break;
            }
        }
        tmp[16] = '\0';
        printf("%s\r\n", tmp);
        if (p-(unsigned char* )buf>=size) {
            break;
        }
    }
}


uint32_t ti;
#define write_int(i) ti = i;fout.write(reinterpret_cast<char*>(&ti), 4)
#define write_string(s) fout << s << '\0'

void test1() {
  auto vm = new VirtualMachine;
  // write test1
  ofstream fout("tmp/test1.wc", ios::in | ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x00000011u); // function_size: 17
  write_int(0x1c020100u); // const r2 1
  write_int(0x1c03ff00u); // const r3 255
  write_int(0x1c000000u); // const r0 0
  write_int(0x240d0003u); // ifgt 13 r0 r3
  write_int(0x1c010000u); // const r1 0
  write_int(0x24090103u); // ifgt 9 r1 r3
  write_int(0x1c040100u); // const r4 1
  write_int(0x1c050200u); // const r5 2
  write_int(0x1c060300u); // const r6 3
  write_int(0x12070506u); // mul r7 r5 r6
  write_int(0x10080407u); // add r8 r4 r7
  write_int(0x1e080000u); // iout r8
  write_int(0x10010102u); // add r1 r1 r2
  write_int(0x21080000u); // back 8
  write_int(0x10000002u); // add r0 r0 r2
  write_int(0x210c0000u); // back 12
  write_int(0x01000000u); // exit
  write_int(0x00000000u); // method_count: 0
  write_int(0x00000000u); // string_count: 0
  fout.close();
  // read test1
  auto main1 = vm->from_WC("tmp/test1");
  // execute test1
  auto start1 = chrono::system_clock::now();
  vm->execute(main1);
  auto end1 = chrono::system_clock::now();
  auto elapsed1 = chrono::duration_cast<chrono::milliseconds>(end1-start1).count();
  printf("\ntime %ld[ms]\n", elapsed1);
  // jit_compile main1
  auto attribute = new vm_function_attribute;
  attribute->type = "void";
  attribute->name = "test1";
  attribute->size = 17;
  attribute->body = main1;
  auto index1 = vm->jit_compile(attribute);
  vm->jit_execute(index1);
}

void test2() {
  auto vm = new VirtualMachine;
  // write test2
  ofstream fout("tmp/test2.wc", ios::in | ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x00000002u); // function_size: 2
  write_int(0x0a000000u); // sout 0
  write_int(0x01000000u); // exit
  write_int(0x00000000u); // method_count: 0
  write_int(0x00000001u); // string_count: 1
  write_int(0x0000000du); // string_size: 13
  write_string("Hello World!");
  fout.close();
  // read test2
  auto main2 = vm->from_WC("tmp/test2");
  // execute test2
  auto start2 = chrono::system_clock::now();
  vm->execute(main2);
  auto end2 = chrono::system_clock::now();
  auto elapsed2 = chrono::duration_cast<chrono::milliseconds>(end2-start2).count();
  printf("\ntime %ld[ms]\n", elapsed2);
  // jit_compile main1
  auto attribute = new vm_function_attribute;
  attribute->type = "void";
  attribute->name = "test2";
  attribute->size = 2;
  attribute->body = main2;
  auto index2 = vm->jit_compile(attribute);
  vm->jit_execute(index2);
}


int main() {
  test1();
  test2();
  return 0;
}

#undef write_int
#undef write_string