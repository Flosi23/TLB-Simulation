#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>

enum LogLevel {
    DEBUG,
    INFO,
    ERROR
};

class Logger {

private:

    LogLevel level = LogLevel::DEBUG;

    void LOG(LogLevel level, const char *fmt, va_list args) {
        if (level < this->level) {
            return;
        }

        char *msg;
        vasprintf(&msg, fmt, args);

        std::cout << msg << std::endl;
    }

public:
    Logger(LogLevel level) : level(level) {}

    Logger(LogLevel level, const char *logfile) : level(level) {
        if (logfile != NULL) {
            // redirect stdout stream to logfile
            freopen(logfile, "w", stdout);
        }
    }

    void DEBUG(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        this->LOG(LogLevel::DEBUG, fmt, args);
        va_end(args);
    }

    void INFO(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        this->LOG(LogLevel::INFO, fmt, args);
        va_end(args);
    }

    void ERROR(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        this->LOG(LogLevel::ERROR, fmt, args);
        va_end(args);
    }
};


#endif
