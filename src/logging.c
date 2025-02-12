#include "logging.h"

// Default settings
static unsigned int enabled_features = LOG_APP | LOG_SOCKET | LOG_HTTP | LOG_ROUTER;
static LogLevel current_log_level = LOG_DEBUG;

// Set enabled features dynamically
void log_set_enabled_features(unsigned int features) {
    enabled_features = features;
}

// Set the minimum log level
void log_set_level(LogLevel level) {
    current_log_level = level;
}

const char* feature_to_str(LogFeature feature) {
    switch (feature) {
    case LOG_APP:
        return "APP";
    case LOG_SOCKET:
        return "SOCKET";
    case LOG_HTTP:
        return "HTTP";
    case LOG_ROUTER:
        return "ROUTER";
    case LOG_DB:
        return "DB";
    default:
        return "UNKNOWN";
    }
}

// Logging function
void log_message_args(LogLevel level, LogFeature feature, const char* format, va_list args) {
    // Check if the feature is enabled
    if (!(enabled_features & feature)) {
        return;
    }

    // If the log level is higher than the current set level, don't log
    if (level < current_log_level) {
        return;
    }

    const char* level_strings[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

    // Print Log Header to stdout for non-error levels, stderr for errors and
    // fatal
    FILE* log_output = (level == LOG_ERROR || level == LOG_FATAL) ? stderr : stdout;

    fprintf(log_output, "[%s][%s] ", level_strings[level], feature_to_str(feature));

    // Print Log Message using vfprintf with the va_list
    vfprintf(log_output, format, args);

    fprintf(log_output, "\n");

    // Handle fatal errors
    if (level == LOG_FATAL) {
        exit(1);
    }
}

void log_message(LogLevel level, LogFeature feature, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message_args(level, feature, format, args);
    va_end(args);
}

void log_debug(LogFeature feature, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message_args(LOG_DEBUG, feature, format, args);
    va_end(args);
}

void log_info(LogFeature feature, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message_args(LOG_INFO, feature, format, args);
    va_end(args);
}

void log_warn(LogFeature feature, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message_args(LOG_WARNING, feature, format, args);
    va_end(args);
}

void log_error(LogFeature feature, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message_args(LOG_ERROR, feature, format, args);
    va_end(args);
}

void log_fatal(LogFeature feature, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message_args(LOG_FATAL, feature, format, args);
    va_end(args);
}
