namespace mcsLog {
  class LogEntry {
    private:
      char *_value;
      int _length;
    public:
      LogEntry(char *entry);
    friend class Logger;
  };

  class Logger {
    private:
      int _logfile_fd;
      const char *_logfile_path;
      long long _logfile_offset;
      long long _logfile_size;
      void *_logfile_mmap_addr;

      void recover();
      ~Logger();
    public:
      Logger(const char* path);
      void *Write(void* value);
      const char *getLogfilePath();
  };

} // namespace mcsLog

