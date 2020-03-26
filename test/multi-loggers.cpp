#include <atomic>
#include <bits/stdc++.h>
#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>

#include "../src/log.hpp"
#include "../utils/macros.hpp"

namespace mcsLog {

  mcsLog::Logger *log[NUM_THREADS];
  std::atomic<int> assignLogCounter;
  unsigned long long writingTime = 0;
  unsigned long long threadCreateTimes = 0;
  unsigned long long dataWritten = NUM_THREADS*VAL_SIZE*ITERATIONS;
  cpu_set_t cpuset;

  static void *dummyFunction(void *value) {
    return NULL;
  }

  static void *writeHelper(void *entry) {
    const char *value = reinterpret_cast<struct LogEntry *>(entry)->getEntry();
    const unsigned long long length = reinterpret_cast<struct LogEntry *>(entry)->getEntryLength();
    if (IS_TEMPORAL) {
      log[assignLogCounter.fetch_add(1)]->Write(value, length);
    } else {
      log[assignLogCounter.fetch_add(1)]->WriteNT(value, length);
    }
  }

  void timePthreadCreates() {
    void *value;
    auto start = std::chrono::high_resolution_clock::now();
    pthread_t thread[NUM_THREADS];
    for (auto i = 0; i < NUM_THREADS; i++) {
      pthread_create(&thread[i], NULL, &dummyFunction, value);
      auto s = pthread_setaffinity_np(thread[i], sizeof(cpu_set_t), &cpuset);
      if (s != 0) {
        std::cout << "Error setting cpu affinity" << std::endl;
      }
    }
    for (auto i = 0; i < NUM_THREADS; i++)
      pthread_join(thread[i], NULL);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    threadCreateTimes = duration.count();
  }

  void InitWriters() {
    char *stringValue = (char *)malloc(sizeof(char)*VAL_SIZE);
    REPEAT(stringValue, VAL_SIZE, 'A');
    const char* value = stringValue;
    for (auto i = 0; i < NUM_THREADS; i++) {
      std::string path = LOG_PATH + std::to_string(i) + ".out";
      log[i] = new mcsLog::Logger(path.c_str(), LOG_SIZE/NUM_THREADS);
    }
    struct mcsLog::LogEntry *entry =  new mcsLog::LogEntry(value);

    // Concurrent threads writing to the log
    auto start = std::chrono::high_resolution_clock::now();
    pthread_t thread[NUM_THREADS];
    for (auto i = 0; i < NUM_THREADS; i++) {
      pthread_create(&thread[i], NULL, &(writeHelper), entry);
      auto s = pthread_setaffinity_np(thread[i], sizeof(cpu_set_t), &cpuset);
      if (s != 0) {
        std::cout << "Error setting cpu affinity" << std::endl;
      }
    }
    for (auto i = 0; i < NUM_THREADS; i++)
      pthread_join(thread[i], NULL);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    writingTime = duration.count();
  }
}

int main() {
  CPU_ZERO(&mcsLog::cpuset);
  for (auto i = 0; i < NUM_THREADS; i++) {
    CPU_SET(i, &mcsLog::cpuset);
  }
  mcsLog::timePthreadCreates();
  mcsLog::InitWriters();
  float bandwidth = (mcsLog::dataWritten * 1.0)/(mcsLog::writingTime - mcsLog::threadCreateTimes);
  std::cout << "isTemporal=" << IS_TEMPORAL << ", " << NUM_THREADS << ", " << VAL_SIZE << " B, ";
  std::cout << ITERATIONS << ", " << (mcsLog::dataWritten * 1.0)/1000000000 << " GiB, ";
  std::cout << ((mcsLog::writingTime - mcsLog::threadCreateTimes)*1.0)/1000000000 << " s, ";
  std::cout << bandwidth << " GiB/s" << std::endl;
}

