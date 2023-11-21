#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "../session/session.h"
#include <stdbool.h>

int user_cmd(session_ptr session, char *arg, int arg_len, char *response);

int pass_cmd(session_ptr session, char *arg, int arg_len, char *response, bool *is_authenticated);

#endif