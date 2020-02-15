#include <atomic>

namespace mcsLog {

  class LogEntry {
    private:
      const char *_value;
      int _length;
    public:
      LogEntry(const char *entry);
      const char *getEntry();
      int getEntryLength();
    friend class Logger;
  };

  class Logger {
    private:
      int _logfile_fd;
      const char *_logfile_path;
      std::atomic<long long> _logfile_offset;
      long long _logfile_size;
      void *_logfile_mmap_addr;

      void recover(long long size);
      void preset();
      ~Logger();
    public:
      Logger(const char* path, long long size);
      void *Write(const char *value, int length);
      const char *getLogfilePath();
  };

} // namespace mcsLog

