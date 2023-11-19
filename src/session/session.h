#ifndef _SESSION_H_
#define _SESSION_H_


#include "connection.h"
#include <stdbool.h>

typedef struct user_session *session_ptr;

session_ptr new_session(connection c);

void delete_user_session(session_ptr session);

void start_session(session_ptr session);

bool session_auth(session_ptr session, char * password);

#endif