#ifndef _SESSION_H_
#define _SESSION_H_

#define BUFFER_SIZE 1024
#define USERNAME_MAX_LEN 40

#include <stdbool.h>
#include "../parser/command_parser.h"
#include <dirent.h>

typedef struct user_session *session_ptr;

typedef enum action_type {
    READING = 0,
    PROCESSING,
    WRITING,
    DONE
} action_type;

#include "../state_machine/stm.h"
#include "../selector/selector.h"

session_ptr new_session(int socket);

void delete_user_session(session_ptr session);

state get_session_state(session_ptr session);

void send_session_response(struct selector_key * key);

void read_session(struct selector_key * key);

int continue_session(session_ptr session);

int get_username(session_ptr session, char * username);

void set_username(session_ptr session, char * username, size_t len);

struct parser_event * get_event(session_ptr session);

void set_dir(session_ptr session, DIR * dir);

fd_handler * get_session_fd_handler(session_ptr session);

DIR * get_dir(session_ptr session);

void init_client_dir(session_ptr session);

int mark_to_delete(session_ptr session, int mail);

void reset_marks(session_ptr session);

#endif