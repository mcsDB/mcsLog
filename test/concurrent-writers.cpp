#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>

#include "../src/log.hpp"

#define NUM_THREADS 4

using namespace std;

void checkLogEntries(mcsLog::Logger *log) {
  string line = "";
  ifstream myfile ((string)log->getLogfilePath());
  if (myfile.is_open()) {
    while (getline (myfile, line))
      cout << line << '\n';
    myfile.close();
  }
}

int main () {
  const char *path = "./log.out";
  struct mcsLog::LogEntry *entry =  new mcsLog::LogEntry("AAAAA");
  mcsLog::Logger *log = new mcsLog::Logger(path);
  // TODO: Concurrent threads writing to the log
  log->Write(entry);
  checkLogEntries(log);
  return 0;
}

