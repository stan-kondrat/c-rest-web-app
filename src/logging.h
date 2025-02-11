#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

typedef enum {
    LOG_APP = 1 << 0,
    LOG_NETWORK = 1 << 1,
    LOG_HTTP = 1 << 2,
    LOG_ROUTER = 1 << 3,
    LOG_DB = 1 << 4,
} LogFeature;

void log_set_enabled_features(unsigned int features);
void log_set_level(LogLevel level);
void log_message(LogLevel level, LogFeature feature, const char* format, ...);

#endif // LOGGING_H
