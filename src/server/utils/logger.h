#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>
#include <stdlib.h>

typedef enum LOG_TYPE {
    LOG_INFO,
    LOG_DEBUG,
    LOG_ERROR,
    LOG_FATAL
} LOG_TYPE;

void log_msg(LOG_TYPE type, const char *msg, ...);

#endif
