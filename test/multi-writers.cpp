#include <bits/stdc++.h>
#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <pthread.h>

#include "../src/log.hpp"
#include "../utils/macros.hpp"

namespace mcsLog {

  mcsLog::Logger *log;
  unsigned long long writingTime = 0;
  unsigned long long threadCreateTimes = 0;
  unsigned long long dataWritten = NUM_THREADS*VAL_SIZE*ITERATIONS;

  static void *dummyFunction(void *value) {
    return NULL;
  }

  static void *writeHelper(void *entry) {
    const char *value = reinterpret_cast<struct LogEntry *>(entry)->getEntry();
    const unsigned long long length = reinterpret_cast<struct LogEntry *>(entry)->getEntryLength();
    log->Write(value, length, true);
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
    const char *path = LOG_PATH;
    char *stringValue = (char *)malloc(sizeof(char)*VAL_SIZE);
    REPEAT(stringValue, VAL_SIZE, 'A');
    const char* value = stringValue;
    log = new mcsLog::Logger(path, LOG_SIZE);
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

/*
void checkLogEntries(mcsLog::Logger *log) {
  unsigned long long count = 0;
  std::string line = "";
  std::ifstream myfile ((std::string)mcsLog::log->getLogfilePath());
  if (myfile.is_open()) {
    while (getline (myfile, line))
      count += (unsigned long long)std::count(line.begin(), line.end(), 'A');
    assert(count == ITERATIONS*VAL_SIZE*NUM_THREADS);
    myfile.close();
  }
}
*/

int main() {
  mcsLog::timePthreadCreates();
  mcsLog::InitWriters();
  float bandwidth = (mcsLog::dataWritten * 1.0)/(mcsLog::writingTime - mcsLog::threadCreateTimes);
  std::cout << "sync=true, " << NUM_THREADS << ", " << VAL_SIZE << " B, ";
  std::cout << ITERATIONS << ", " << (mcsLog::dataWritten * 1.0)/1000000000 << " GiB, ";
  std::cout << ((mcsLog::writingTime - mcsLog::threadCreateTimes)*1.0)/1000000000 << " s, ";
  std::cout << bandwidth << " GiB/s" << std::endl;
  /*
  std::cout << "#Threads  : " << NUM_THREADS << std::endl;
  std::cout << "ValueSize : " << VAL_SIZE << std::endl;
  std::cout << "Iterations: " << ITERATIONS << std::endl;
  std::cout << "Bandwidth : " << bandwidth << " GiB/s" << std::endl;
  std::cout << "TotalTime : " << ((mcsLog::writingTime - mcsLog::threadCreateTimes)*1.0)/1000000000  << " s"<< std::endl;
  std::cout << "TotalData : " << (mcsLog::dataWritten * 1.0)/1000000000 << " GiB" << std::endl;
  // checkLogEntries(log);
  */
}

