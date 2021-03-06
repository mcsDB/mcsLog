#include <immintrin.h>
#include <cstring>
#include <fcntl.h>
#include <immintrin.h>
#include <iostream>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

#include "log.hpp"
#include "nontemporal.h"
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

  void Logger::preset() {
    memset(_logfile_mmap_addr, '-', _logfile_size);
  }

  Logger::Logger(const char *path, long long size) {
    _logfile_path = path;
    // Sets _logfile_fd, _logfile_size, and _logfile_offset
    recover(size);
    _logfile_mmap_addr = mmap(NULL, _logfile_size, PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_POPULATE, _logfile_fd, 0);
    preset();
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

  void *Logger::Write(const char *value, int length) {
    for (auto iter = 0; iter < ITERATIONS; iter++) {
      // TODO: If write_offset is beyond resize_threshold, extend the file size
      std::memcpy((void *)((unsigned long)_logfile_mmap_addr + _logfile_offset), value, length);
      for (auto cl = 0; cl < length; cl += CLFLUSH_SIZE) {
        _mm_clwb((unsigned long)_logfile_mmap_addr + _logfile_offset + cl);
      }
      _mm_sfence();
      // std::memset((void *)((unsigned long)_logfile_mmap_addr + _logfile_offset), 'A', length);
      _logfile_offset += length;
    }
  }

  void *Logger::WriteNT(const char *value, int length) {
    for (auto iter = 0; iter < ITERATIONS; iter++) {
      MOVNT((void *)((unsigned long)_logfile_mmap_addr + _logfile_offset), value, length);
      _mm_sfence();
      _logfile_offset += length;
    }
  }

} // namespace mcsLog

