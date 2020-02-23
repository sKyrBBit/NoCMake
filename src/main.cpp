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

int main(int argc, char** argv) {
  if (argv[1] == string("default")) {
	cout << "usage | make ARG=[file]" << endl;
  } else {
    auto vm = new VirtualMachine;
    vm->from_LL(string(argv[1]));
    vm->execute();
  }
  return 0;
}