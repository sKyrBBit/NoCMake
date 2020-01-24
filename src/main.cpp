#include "rcwt.h"
#include "VirtualMachine.h"
#include <chrono>

using namespace std;

uint32_t ti;
#define write_int(i) ti = i;fout.write((char*) &ti, 4)

int main() {
  ofstream fout("tmp/test.wc", ios::in | ios::binary);
  write_int(0xdeadbeefu); // magic
  write_int(0x0000000du); // function_size: 13
  write_int(0x0000001cu); // const r0 0
  write_int(0x0001011cu); // const r1 1
  write_int(0x00ff021cu); // const r2 255
  write_int(0x02000924u); // ifgt 9 r0 r2
  write_int(0x0001031cu); // const r3 1
  write_int(0x0002041cu); // const r4 2
  write_int(0x0003051cu); // const r5 3
  write_int(0x05040612u); // mul r6 r4 r5
  write_int(0x06030710u); // add r7 r3 r6
  write_int(0x0000071eu); // out r7
  write_int(0x01000010u); // add r0 r0 r1
  write_int(0x00000821u); // back 8
  write_int(0x00000001u); // exit
  write_int(0x00000000u); // method_count: 0
  fout.flush();
  
  auto start = chrono::system_clock::now();

  auto vm = new VirtualMachine;
  auto main = vm->from_WC("tmp/test");
  vm->execute(main);
  
  auto end = chrono::system_clock::now();
  double elapsed = chrono::duration_cast<chrono::milliseconds>(end-start).count();
  printf("time %lf[ms]\n", elapsed);
  return 0;
}
