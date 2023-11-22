#include "session.h"
#include "../state_machine/stm.h"
#include "connection.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../parser/command_parser.h"
#include "../buffer/buffer.h"
#include "../server/server_utils.h"
#include "../utils/stack_ADT.h"
//#include "../server/server_ADT.h"

struct user_mail_dir {
    DIR * dir_ptr;
    int * mails;
    int mails_count;
    int dir_index;
};

typedef struct user_session {
    int socket;
    buffer read_buffer;
    char read_buffer_data[BUFFER_SIZE];
    buffer write_buffer;
    char write_buffer_data[BUFFER_SIZE];
    state_machine_ptr stm;
    char username[USERNAME_MAX_LEN];
    struct parser * parser;
    struct parser_event * event;
    struct user_mail_dir * dir;
    struct fd_handler * fd_handler;
    int write_bytes;
    stack_adt actions;
} user_session;


session_ptr new_session(int socket) {
    session_ptr session = malloc(sizeof(user_session));
    session->socket = socket;
    session->stm = state_machine_init();
    memset(session->username, 0, USERNAME_MAX_LEN);
    session->parser = command_parser_init();
    session->event = get_command_parser_event(session->parser);
    session->fd_handler = calloc(1, sizeof(struct fd_handler));
    session->fd_handler->handle_read = read_session;
    session->fd_handler->handle_write = send_session_response;
    session->fd_handler->handle_close = close_session;
    session->write_bytes = 0;
    session->actions = new_stack();
    push_action(session, READ);
    push_action(session, PROCESSING);
    buffer_init(&session->read_buffer, BUFFER_SIZE, (uint8_t *)session->read_buffer_data);
    buffer_init(&session->write_buffer, BUFFER_SIZE, (uint8_t *)session->write_buffer_data);
    session->dir = calloc(1, sizeof(struct user_mail_dir));
    session->dir->dir_index = 1;
    return session;
}

void delete_user_session(session_ptr session) {
    close(session->socket);
    remove_user(session);
    free_state_machine(session->stm);
    command_parser_destroy(session->parser);
    delete_stack(session->actions);
    closedir(session->dir->dir_ptr);
    free(session->dir->mails);
    free(session->dir);
    free(session->fd_handler);
    free(session);
}

state get_session_state(session_ptr session) {
    return get_state(session->stm);
}

void read_session(struct selector_key * key) {
    session_ptr session = (session_ptr) key->data;

    action_type current_action = peek_action(session);

    if (session->event->type != MAYEQ) {
        command_parser_reset(session->parser);
    }

    if (current_action == READ) {
        size_t wbytes = 0;
        char * wbuffer = (char *) buffer_write_ptr(&session->read_buffer, &wbytes);
        int bytes_received = w_recv(session->socket, wbuffer, wbytes, 0);
        buffer_write_adv(&session->read_buffer, bytes_received);
    }

    size_t rbytes = 0;
    size_t bytes_read = 0;
    char * rbuffer = (char *) buffer_read_ptr(&session->read_buffer, &rbytes);
    session->event = get_command(session->event, session->parser, rbuffer, rbytes, &bytes_read);
    buffer_read_adv(&session->read_buffer, bytes_read);

    if (session->event->type != MAYEQ) {
        if (buffer_can_read(&session->read_buffer)) {
            push_action(session, READING);
        } else if (current_action == READING) {
            pop_action(session);
        }   
        push_action(session, PROCESS);
        session->write_bytes = continue_session(session);
        selector_set_interest_key(key, OP_WRITE);
    }
}

void send_session_response(struct selector_key * key) {
    session_ptr session = key->data;

    action_type current_action = peek_action(session);

    if (current_action == PROCESSING || current_action == PROCESS) {
        session->write_bytes = continue_session(session);
        return;
    }

    if (current_action == READING) {
        read_session(key);
        return;
    }
    if (session->write_bytes > 0) {
        size_t rbytes = 0;
        char * rbuffer = (char *) buffer_read_ptr(&session->write_buffer, &rbytes);
        int bytes_sent = w_send(session->socket, rbuffer, rbytes, 0);
        buffer_read_adv(&session->write_buffer, bytes_sent);
        session->write_bytes -= bytes_sent;
    }
    if (session->write_bytes != 0) {
        if (current_action == WRITE) {
            pop_action(session);
        }
        if (current_action != WRITING) {
            push_action(session, WRITING);
        }
        return;
    }
    if (current_action == WRITE || current_action == WRITING) {
        pop_action(session);
        current_action = peek_action(session);  
    }
    if (current_action == READ) {
        selector_set_interest_key(key, OP_READ);
        return;
    }
    if (get_session_state(session) == EXIT) {
        selector_unregister_fd(key->s, key->fd);
        return;
    }
}

void close_session(struct selector_key * key) {
    if (key->data != NULL)
        delete_user_session(key->data);
}

int continue_session(session_ptr session) {
    size_t wbytes = 0;
    char * wbuffer = (char *) buffer_write_ptr(&session->write_buffer, &wbytes);
    int bytes_written = state_machine_run(session->stm, session, wbuffer, wbytes);
    buffer_write_adv(&session->write_buffer, bytes_written);
    push_action(session, WRITE);
    return bytes_written;
}

int get_username(session_ptr session, char * username) {
    int len = strlen(session->username);
    if (len == 0) {
        return -1;
    }
    strncpy(username, session->username, len);
    return len;
}

void set_username(session_ptr session, char * username, size_t len) {
    strncpy(session->username, username, len);
}

struct parser_event * get_event(session_ptr session) {
    return session->event;
}

void set_dir(session_ptr session, DIR * dir) {
    session->dir->dir_ptr = dir;
}

DIR * get_dir(session_ptr session) {
    return session->dir->dir_ptr;
}

static int get_file_count(DIR *dir) {
    if (dir == NULL)
        return -1;

    struct dirent *entry;
    int result = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG)
            result++;
    }

    return result;
}


void init_user_dir(session_ptr session) {
    int file_count = get_file_count(session->dir->dir_ptr);
    session->dir->mails = (int *) calloc(file_count, sizeof(int));
    session->dir->mails_count = file_count;
}

static char is_marked_to_delete(session_ptr session, int mail) {
    return session->dir->mails[mail -1] == true;
}

int mark_to_delete(session_ptr session, int mail) {
    if( is_marked_to_delete(session, mail) || !((mail) > (0) && (mail) <= (session->dir->mails_count)) ) {
        return -1;
    }

    session->dir->mails[mail - 1] = true;
    return 0;
}

void reset_marks(session_ptr session) {
    memset(session->dir->mails,0,sizeof(session->dir->mails[0]) * session->dir->mails_count);
}

int * get_dir_mails(session_ptr session) {
    return session->dir->mails;
}

fd_handler * get_session_fd_handler(session_ptr session) {
    return session->fd_handler;
}

action_type pop_action(session_ptr session) {
    data_t action;
    int ret = pop(session->actions, &action);
    return ret == -1 ? ret : action.elem;
}

void push_action(session_ptr session, action_type action) {
    data_t data;
    data.elem = action;
    push(session->actions, data);
}

action_type peek_action(session_ptr session) {
    data_t action;
    int ret = peek(session->actions, &action);
    return ret == -1 ? ret : action.elem;
}

int get_user_dir_idx(session_ptr session) {
    return session->dir->dir_index;
}

void set_user_dir_idx(session_ptr session, int idx) {
    session->dir->dir_index = idx;
}