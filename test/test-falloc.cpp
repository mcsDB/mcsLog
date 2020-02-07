#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <sys/mman.h>
#include <iostream>

using namespace std;

int main() {
      int fd = open("foo", O_RDWR|O_CREAT, 0666);
      if (fd < 0)
        return 1;
      posix_fallocate(fd, 0, 400000);
      void* addr = mmap(NULL, 400000, PROT_READ | PROT_WRITE,
          MAP_PRIVATE | MAP_POPULATE, fd, 0);
      if (addr == 0)
        cout << "mmap failed" << endl;
      const char *buffer1 = "AAAA";
      const char *buffer2 = "BBBB";
      write(fd, buffer1, 6);
      auto start = std::chrono::high_resolution_clock::now();
      auto end = start;
      auto duration = end - start;
      for (auto i = 0; i < 1000000000000; i++) {
        start = std::chrono::high_resolution_clock::now();
        auto ret = memcpy((void *)((unsigned long long)addr), buffer2, 6);
        end = std::chrono::high_resolution_clock::now();
        duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        if (ret != addr)
          cout << "memcpy failed" << endl;
      }
      std::cout << duration.count() << std::endl;
}
