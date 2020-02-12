#include <math.h>
#ifdef WITH_OPENMP
#include <omp.h>
#endif  // WITH_OPENMP
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "./monotonic_timer.h"

#define SAMPLES 1
#define TIMES 1024*1024
#define BYTES_PER_GB (1024*1024*1024LL)
#define BYTES_PER_MB (1024*1024LL)
#define BYTES_PER_KB (1024LL)
#define SIZE (1*BYTES_PER_KB)
#define PAGE_SIZE (1<<12)
#define timefun(f) timeit(f, #f)

// This must be at least 32 byte aligned to make some AVX instructions happy.
// Have PAGE_SIZE buffering so we don't have to do math for prefetching.
char* array = (char *)malloc(sizeof(char)*(SIZE*TIMES + PAGE_SIZE));
// __attribute__((aligned (32)));

void write_memory_memset(void* array, size_t size) {
    memset(array, 0xff, size);
}

// Compute the bandwidth in GiB/s.
static inline double to_bw(size_t bytes, double secs) {
  double size_bytes = (double) bytes;
  double size_gb = size_bytes / ((double) BYTES_PER_GB);
  return size_gb / secs;
}

void timeit(void (*function)(void*, size_t), char* name) {
  size_t i;
  double before, after, total;
  for (i = 0; i < SAMPLES; i++) {

    before = monotonic_time(); 
    int j;
    for (j = 0; j < TIMES; j++) {
      function(array + j*SIZE, SIZE);
      // function(array, SIZE);
    }
    after = monotonic_time();

    total = after - before;
  }

  printf("---%32s: %5.2f GiB/s, %d, %d\n", name, to_bw(SIZE * TIMES, total), SIZE, SAMPLES);
}

int main() {
  memset(array, 0xFF, SIZE*TIMES);  // un-ZFOD the page.
    * ((uint64_t *) &array[SIZE]) = 0;
  timefun(write_memory_memset);
}
