#define LOG_PATH "log.out"
#define NUM_THREADS 4
#define VAL_SIZE 1024
#define ITERATIONS 1024*1024ULL
#define LOG_SIZE ITERATIONS*NUM_THREADS*VAL_SIZE
#define REPEAT(buffer, size, value) memset(buffer, value, size)
