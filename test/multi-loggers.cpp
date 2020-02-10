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
  int assignLogCounter = 0;
  unsigned long long writingTime = 0;
  unsigned long long threadCreateTimes = 0;
  unsigned long long dataWritten = NUM_THREADS*VAL_SIZE*ITERATIONS;

  static void *dummyFunction(void *value) {
    return NULL;
  }

  static void *writeHelper(void *entry) {
    const char *value = reinterpret_cast<struct LogEntry *>(entry)->getEntry();
    const unsigned long long length = reinterpret_cast<struct LogEntry *>(entry)->getEntryLength();
    log[assignLogCounter++]->Write(value, length, false);
  }

  void timePthreadCreates() {
    void *value;
    auto start = std::chrono::high_resolution_clock::now();
    pthread_t thread[NUM_THREADS];
    for (auto i = 0; i < NUM_THREADS; i++)
      pthread_create(&thread[i], NULL, &dummyFunction, value);
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
      std::string path = std::to_string(i) + LOG_PATH;
      log[i] = new mcsLog::Logger(path.c_str());
    }
    struct mcsLog::LogEntry *entry =  new mcsLog::LogEntry(value);

    // Concurrent threads writing to the log
    auto start = std::chrono::high_resolution_clock::now();
    pthread_t thread[NUM_THREADS];
    for (auto i = 0; i < NUM_THREADS; i++)
      pthread_create(&thread[i], NULL, &(writeHelper), entry);
    for (auto i = 0; i < NUM_THREADS; i++)
      pthread_join(thread[i], NULL);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    writingTime = duration.count();
  }
}

int main() {
  mcsLog::timePthreadCreates();
  mcsLog::InitWriters();
  float bandwidth = (mcsLog::dataWritten * 1.0)/(mcsLog::writingTime - mcsLog::threadCreateTimes);
  std::cout << "Bandwidth : " << bandwidth << " GiB/s" << std::endl;
  std::cout << "TotalTime : " << ((mcsLog::writingTime - mcsLog::threadCreateTimes)*1.0)/1000000000  << " s"<< std::endl;
  std::cout << "TotalData : " << (mcsLog::dataWritten * 1.0)/1000000000 << " GiB" << std::endl;
  // checkLogEntries(log);
}

