#define LOGSIZE 128*1024*1024

#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log.hpp"

namespace mcsLog {

    Logger::Logger(const char* path) {
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
    }

    void Logger::recover() {
        _logfile_fd = open(_logfile_path, O_RDWR, 0666);
        if (_logfile_fd < 0) {
            _logfile_offset = 0;
            _logfile_fd = open(_logfile_path, O_RDWR | O_CREAT, 0666);
            if (_logfile_fd < 0)
                throw std::runtime_error("Error: Could not open the logfile");
            // [TODO]: Make this environment independent?
            int falloc_ret = posix_fallocate(_logfile_fd, 0, LOGSIZE);
            _logfile_size = LOGSIZE;
        }
        else {
            // [TODO]: Implement recovery on the existing logfile
            // For now, rewriting the logfile - setting the offset to 0
            _logfile_offset = 0;
            _logfile_size = LOGSIZE;
        }
    }

    unsigned long long Logger::Write(void* value, int length) {        
        long long write_offset = __sync_fetch_and_add(&_logfile_offset, length);
        // [TODO]: If write_offset is beyond resize_threshold, extend the file size
        std::memcpy((void *)(_logfile_mmap_addr + write_offset), value, length);
    }

} // namespace mcsLog