#include "session.h"
#include "../state_machine/stm.h"
#include "connection.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../parser/command_parser.h"
#include "../buffer/buffer.h"
#include "../server/server_utils.h"

#define BUFFER_SIZE 1024

typedef struct user_session {
    int socket;
    buffer read_buffer;
    char read_buffer_data[BUFFER_SIZE];
    buffer write_buffer;
    char write_buffer_data[BUFFER_SIZE];
    state_machine_ptr stm;
    char * username;
    struct parser * parser;
    struct parser_event * event;
} user_session;

user_session * new_session(int socket) {
    user_session * session = malloc(sizeof(user_session));
    session->socket = socket;
    buffer_init(&session->read_buffer, BUFFER_SIZE, (uint8_t *)session->read_buffer_data);
    buffer_init(&session->write_buffer, BUFFER_SIZE, (uint8_t *)session->write_buffer_data);
    session->stm = state_machine_init();
    session->username = NULL;
    session->parser = command_parser_init();
    session->event = malloc(sizeof(struct parser_event));
    return session;
}

void delete_user_session(user_session * session) {
    free(session->username);
    free(session);
}

state get_session_state(user_session * session) {
    return get_state(session->stm);
}

void write_session(user_session * session, size_t len) {
    while(len > 0) {
        size_t rbytes = 0;
        char * rbuffer = (char *) buffer_read_ptr(&session->write_buffer, &rbytes);
        int wbytes = w_send(session->socket, rbuffer, rbytes, 0);
        buffer_read_adv(&session->write_buffer, wbytes);
        len -= wbytes;
    }
}

struct parser_event * read_session(user_session * session) {
    session->event = malloc(sizeof(struct parser_event));

    while(session->event->type == MAYEQ) {
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
    parser_reset(session->parser);
    return session->event;
}

int continue_session(user_session * session) {
    size_t wbytes = 0;
    char * wbuffer = (char *) buffer_write_ptr(&session->write_buffer, &wbytes);
    int bytes_written = state_machine_run(session->stm, session->event, wbuffer, wbytes);
    buffer_write_adv(&session->write_buffer, bytes_written);
    return bytes_written;
}