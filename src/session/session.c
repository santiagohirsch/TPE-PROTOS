#include "session.h"
#include "../state_machine/stm.h"
#include "connection.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../parser/command_parser.h"
#include "../buffer/buffer.h"
#include "../server/server_utils.h"

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
    DIR * dir;
    struct fd_handler * fd_handler;
    int write_bytes;
} user_session;

session_ptr new_session(int socket) {
    session_ptr session = malloc(sizeof(user_session));
    session->socket = socket;
    buffer_init(&session->read_buffer, BUFFER_SIZE, (uint8_t *)session->read_buffer_data);
    buffer_init(&session->write_buffer, BUFFER_SIZE, (uint8_t *)session->write_buffer_data);
    session->stm = state_machine_init();
    memset(session->username, 0, USERNAME_MAX_LEN);
    session->parser = command_parser_init();
    session->event = malloc(sizeof(struct parser_event));
    session->fd_handler = malloc(sizeof(struct fd_handler));
    session->fd_handler->handle_read = read_session;
    session->fd_handler->handle_write = send_session_response;
    session->write_bytes = 0;
    return session;
}

void delete_user_session(session_ptr session) {
    free(session->username);
    free(session);
}

state get_session_state(session_ptr session) {
    return get_state(session->stm);
}

void read_session(struct selector_key * key) {
    session_ptr session = (session_ptr) key->data;

    if (session->event->type != MAYEQ) {
        session->event->command_len = 0;
        session->event->arg1_len = 0;
        session->event->arg2_len = 0;
        session->event = malloc(sizeof(struct parser_event));
        parser_reset(session->parser);
    }

    if (session->event->type == MAYEQ) {
        if (!buffer_can_read(&session->read_buffer)) {
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
    }

    if (session->event->type != MAYEQ) {
        session->write_bytes = continue_session(session);
        selector_set_interest_key(key, OP_WRITE);
    }
}

int write_session_response(session_ptr session, char * response, size_t len) {
    size_t wbytes = 0;
    char * wbuffer = (char *) buffer_write_ptr(&session->write_buffer, &wbytes);
    int bytes_to_write = len <= wbytes ? len : wbytes;
    strncpy(wbuffer, response, bytes_to_write);
    buffer_write_adv(&session->write_buffer, bytes_to_write);
    return bytes_to_write;
}

void send_session_response(struct selector_key * key) {
    session_ptr session = (session_ptr) key->data;
    if (session->write_bytes == 0 && buffer_can_read(&session->write_buffer)) {
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
    if (session->write_bytes == 0 && !buffer_can_read(&session->write_buffer)) {
        selector_set_interest_key(key, OP_READ);
        return;
    }
}

int continue_session(session_ptr session) {
    size_t wbytes = 0;
    char * wbuffer = (char *) buffer_write_ptr(&session->write_buffer, &wbytes);
    int bytes_written = state_machine_run(session->stm, session, wbuffer, wbytes);
    buffer_write_adv(&session->write_buffer, bytes_written);
    return bytes_written;
}

int get_username(session_ptr session, char * username) {
    if (strlen(session->username) == 0) {
        return -1;
    }
    strncpy(username, session->username, strlen(session->username));
    return 0;
}

void set_username(session_ptr session, char * username, size_t len) {
    strncpy(session->username, username, len);
}

struct parser_event * get_event(session_ptr session) {
    return session->event;
}

void set_dir(session_ptr session, DIR * dir) {
    session->dir = dir;
}

fd_handler * get_session_fd_handler(session_ptr session) {
    return session->fd_handler;
}