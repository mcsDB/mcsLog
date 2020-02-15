#define LOG_PATH "/mnt/pmemdir/log"
#define NUM_THREADS 48
#define VAL_SIZE 4194304LL
#define ITERATIONS 5LL
#define LOG_SIZE 1073741824LL
// b - buffer, s - size, v - value
#define REPEAT(b, s, v) memset(b, v, s)
