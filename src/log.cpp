#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

#include "log.hpp"
#include "../utils/macros.hpp"

namespace mcsLog {

  LogEntry::LogEntry(const char *entry) {
    _value = entry;
    _length = strlen(entry);
  }

  const char *LogEntry::getEntry() {
    return _value;
  }

  int LogEntry::getEntryLength() {
    return _length;
  }

  Logger::Logger(const char *path, long long size) {
    _logfile_path = path;
    // Sets _logfile_fd, _logfile_size, and _logfile_offset
    recover(size);
    _logfile_mmap_addr = mmap(NULL, _logfile_size, PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_POPULATE, _logfile_fd, 0);
    if (_logfile_mmap_addr == 0) {
      throw std::runtime_error("Error: Could not mmap the logfile");
    }
  }

  Logger::~Logger() {
    munmap(_logfile_mmap_addr, _logfile_size);
    close(_logfile_fd);
  }

  const char *Logger::getLogfilePath() {
    return _logfile_path;
  }

  void Logger::recover(long long size) {
    _logfile_fd = open(_logfile_path, O_RDWR, 0666);
    if (_logfile_fd < 0) {
      _logfile_offset = 0;
      _logfile_fd = open(_logfile_path, O_RDWR | O_CREAT, 0666);
      if (_logfile_fd < 0) {
        std::cout << _logfile_path << std::endl;
        throw std::runtime_error("Error: Could not open the logfile ");
      }
        // TODO: Make this environment independent?
      int falloc_ret = posix_fallocate(_logfile_fd, 0, size);
      if (falloc_ret < 0)
        throw std::runtime_error("Error: Could not allocate space for the logfile");
      _logfile_size = size;
    }
    else {
      _logfile_offset = 0;
      _logfile_size = size;
      // TODO: Implement recovery on the existing logfile
      // For now, rewriting the logfile - setting the offset to 0
      throw std::runtime_error("Error: Recovery is not implemented");
    }
  }

  void *Logger::Write(const char *value, int length, bool threadSync) {
    for (auto iter = 0; iter < ITERATIONS; iter++) {
      long long write_offset;
      if (threadSync) {
        write_offset = _logfile_offset.fetch_add(length);
      }
      else {
        write_offset = _logfile_offset;
        _logfile_offset += length;
      }
      // TODO: If write_offset is beyond resize_threshold, extend the file size
      std::memcpy((void *)((unsigned long)_logfile_mmap_addr + write_offset), value, length);
    }
  }

} // namespace mcsLog

