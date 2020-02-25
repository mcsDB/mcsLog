#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <inttypes.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <emmintrin.h>
#include <stdbool.h>
//#include "nv_common.h"

#define _mm_clwb(addr)\
      asm volatile("clwb (%0)\n\t" : : "r"(addr) : "memory");

#define CHUNK_SIZE_GRANULARITY 64
#define CHUNK_SIZE      128 /* 16*8 */
#define CHUNK_SHIFT     7
#define CHUNK_SHIFT_GRANULARITY 6
#define CHUNK_MASK      (CHUNK_SIZE - 1)
#define CHUNK_MASK_GRANULARITY (CHUNK_SIZE_GRANULARITY - 1)

#define DWORD_SIZE      4
#define DWORD_SHIFT     2
#define DWORD_MASK      (DWORD_SIZE - 1)

#define MOVNT_SIZE      16
#define MOVNT_MASK      (MOVNT_SIZE - 1)
#define MOVNT_SHIFT     4
#define FLUSH_ALIGN  ((uintptr_t)64)
#define ALIGN_MASK      (FLUSH_ALIGN - 1)

#define MOVNT_THRESHOLD 256
#define MOVNT_THRESHOLD_GRANULARITY 64
#define MOVNT memmove_nodrain_movnt_granularity

#define CLFLUSH_SIZE 64
#define _mm_clflushopt(addr)\
  asm volatile("clflushopt %0" : "+m" (*(volatile char *)(addr)))

void *memmove_nodrain_movnt_granularity(void *pmemdest, const void *src, size_t len);
