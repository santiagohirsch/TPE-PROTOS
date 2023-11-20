#ifndef _SESSION_H_
#define _SESSION_H_


#include "../state_machine/stm.h"
#include <stdbool.h>

typedef struct user_session *session_ptr;

session_ptr new_session(int socket);

void delete_user_session(session_ptr session);

state get_session_state(session_ptr session);

void send_session_response(session_ptr session, size_t len);

int write_session_response(session_ptr session, char * response, size_t len);

struct parser_event * read_session(session_ptr session);

int continue_session(session_ptr session);

int get_username(session_ptr session, char * username);

void set_username(session_ptr session, char * username, size_t len);
#endif