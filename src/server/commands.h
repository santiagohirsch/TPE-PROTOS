#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "../session/session.h"

int user_cmd(session_ptr session, char *arg, int arg_len, char *response);

void pass_cmd(session_ptr session, char *arg, int arg_len, char *response);

#endif