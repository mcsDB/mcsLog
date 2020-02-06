#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/mman.h>
#include <iostream>

using namespace std;

int main() {
      int fd = open("foo", O_RDWR|O_CREAT, 0666);
      if (fd < 0)
        return 1;
      posix_fallocate(fd, 0, 400000);
      void* addr = mmap(NULL, 400000, PROT_READ | PROT_WRITE,
          MAP_SHARED | MAP_POPULATE, fd, 0);
      if (addr == 0)
        cout << "mmap failed" << endl;
      const char *buffer1 = "AAAAA";
      const char *buffer2 = "BBBBB";
      write(fd, buffer1, 6);
      auto ret = memcpy((void *)((unsigned long long)addr), buffer2, 6); 
      if (ret != addr)
        cout << "memcpy failed" << endl;
}
