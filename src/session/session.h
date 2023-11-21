#ifndef _SESSION_H_
#define _SESSION_H_

#define BUFFER_SIZE 1024
#define USERNAME_MAX_LEN 40

#include <stdbool.h>
#include "../parser/command_parser.h"
#include <dirent.h>

typedef struct user_session *session_ptr;

typedef enum action_type {
    READ = 0,
    READING,
    PROCESS,
    PROCESSING,
    WRITE,
    WRITING
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

void init_user_dir(session_ptr session);

int mark_to_delete(session_ptr session, int mail);

void reset_marks(session_ptr session);

action_type pop_action(session_ptr session);

void push_action(session_ptr session, action_type action);

action_type peek_action(session_ptr session);

int get_user_dir_idx(session_ptr session);

void set_user_dir_idx(session_ptr session, int idx);

int * get_dir_mails(session_ptr session);

#endif