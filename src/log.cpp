#include <cstring>
#include <fcntl.h>
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

  Logger::Logger(const char *path) {
    _logfile_path = path;
    // Sets _logfile_fd, _logfile_size, and _logfile_offset
    recover();
    _logfile_mmap_addr = mmap(NULL, _logfile_size, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_POPULATE, _logfile_fd, 0);
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

  void Logger::recover() {
    _logfile_fd = open(_logfile_path, O_RDWR, 0666);
    if (_logfile_fd < 0) {
      _logfile_offset = 0;
      _logfile_fd = open(_logfile_path, O_RDWR | O_CREAT, 0666);
      if (_logfile_fd < 0)
        throw std::runtime_error("Error: Could not open the logfile");
      // TODO: Make this environment independent?
      int falloc_ret = posix_fallocate(_logfile_fd, 0, LOG_SIZE);
      if (falloc_ret < 0)
        throw std::runtime_error("Error: Could not allocate space for the logfile");
      _logfile_size = LOG_SIZE;
    }
    else {
      _logfile_offset = 0;
      _logfile_size = LOG_SIZE;
      // TODO: Implement recovery on the existing logfile
      // For now, rewriting the logfile - setting the offset to 0
      throw std::runtime_error("Error: Recovery is not implemented");
    }
  }

  void *Logger::Write(const char *value, int length) {
    for (auto iter = 0; iter < ITERATIONS; iter++) {
      long long write_offset = _logfile_offset.fetch_add(length);
      // TODO: If write_offset is beyond resize_threshold, extend the file size
      std::memcpy((void *)((unsigned long)_logfile_mmap_addr + write_offset), value, length);
    }
  }

} // namespace mcsLog

