#include "logger.h"
#include <stdarg.h>

static char *type_to_str(LOG_TYPE type) {
    switch (type) {
        case LOG_INFO:
            return "INFO";
        case LOG_DEBUG:
            return "DEBUG";
        case LOG_ERROR:
            return "ERROR";
        case LOG_FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

void log_msg(LOG_TYPE type, const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    char *type_str = type_to_str(type);
    
    FILE * write_to = stdout;
    if (type == LOG_ERROR || type == LOG_FATAL)
    {
        write_to = stderr;
    }
    
    fprintf(write_to, "[%s] %s:%d. msg-> ", type_str, __FILE__, __LINE__);
    vfprintf(write_to, msg, args);
    fprintf(write_to, "\n");

    va_end(args);

    if (type == LOG_FATAL) {
        fprintf(write_to, "Exit program...\n");
        exit(EXIT_FAILURE);
    }
}
