namespace mcsLog {
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
        unsigned long long Write(void* value, int length);
    };
} // namespace mcsLog

