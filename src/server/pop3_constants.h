#ifndef _POP3_CONSTANTS_H_
#define _POP3_CONSTANTS_H_

#include <limits.h>

// Maximum length of a MAIL PATH
#define MAX_MAIL_PATH_LEN (PATH_MAX - 2 * NAME_MAX)

#define MAX_RESPONSE_LEN 512 // RFC 1939 

#define BUFFER_SIZE (8 * 1024)

#endif
