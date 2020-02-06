#include <cstring>
#include <pthread.h>
#include <stdexcept>
#include <stdlib.h>

#include "../src/log.hpp"

#define NUM_THREADS 4
#define VAL_SIZE 1024
#define REPEAT(buffer, size, value) memset(&buffer, value, size)

namespace logTesting {

    int main () {
        const char *path = "./log.out";
        char value[VAL_SIZE];
        REPEAT(value, VAL_SIZE, 'A');
        mcsLog::Logger *log = new mcsLog::Logger(path);

        pthread_t threads[NUM_THREADS];
        int threadCount, createReturnValue;
        for (threadCount = 0; threadCount <= NUM_THREADS; threadCount++) {
            createReturnValue = pthread_create(&threads[threadCount], NULL,
             log->Write, (void *)value);
            if (createReturnValue != 0) {
                throw std::runtime_error("Error: Could not create threads");
            }
        }
    }

} // namespace logTesting
