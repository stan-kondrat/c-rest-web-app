#include "logging.h"

// Default settings
static unsigned int enabled_features =
    LOG_APP | LOG_NETWORK | LOG_HTTP | LOG_ROUTER;
static LogLevel current_log_level = LOG_DEBUG;

// Set enabled features dynamically
void log_set_enabled_features(unsigned int features) {
    enabled_features = features;
}

// Set the minimum log level
void log_set_level(LogLevel level) {
    current_log_level = level;
}

// Logging function
void log_message(LogLevel level, LogFeature feature, const char* format, ...) {
    // Check if the feature is enabled
    if (!(enabled_features & feature)) {
        return;
    }

    // If the log level is higher than the current set level, don't log
    if (level < current_log_level) {
        return;
    }

    // Log Level Strings
    const char* level_strings[] = {"DEBUG", "INFO", "WARNING", "ERROR",
                                   "FATAL"};

    // Print Log Header to stdout for non-error levels, stderr for errors and
    // fatal
    FILE* log_output =
        (level == LOG_ERROR || level == LOG_FATAL) ? stderr : stdout;

    fprintf(log_output, "[%s] ", level_strings[level]);

    // Print Log Message
    va_list args;
    va_start(args, format);
    vfprintf(log_output, format, args);
    va_end(args);

    fprintf(log_output, "\n");

    // Handle fatal errors
    if (level == LOG_FATAL) {
        exit(1);
    }
}
