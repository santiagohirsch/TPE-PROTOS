#include "session.h"
#include "../state_machine/stm.h"
#include "connection.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../parser/command_parser.h"
#include "../buffer/buffer.h"
#include "../server/server_utils.h"

struct client_dir {
    DIR * dir_ptr;
    int * mails;
    int mails_count;
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
    struct client_dir * dir;
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
    return session;
}

void delete_user_session(session_ptr session) {
    free(session->username);
    free(session);
}

state get_session_state(session_ptr session) {
    return get_state(session->stm);
}

void send_session_response(session_ptr session, size_t len) {
    while(len > 0) {
        size_t rbytes = 0;
        char * rbuffer = (char *) buffer_read_ptr(&session->write_buffer, &rbytes);
        int wbytes = w_send(session->socket, rbuffer, rbytes, 0);
        buffer_read_adv(&session->write_buffer, wbytes);
        len -= wbytes;
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

struct parser_event * read_session(session_ptr session) {
    session->event->command_len = 0;
    session->event->arg1_len = 0;
    session->event->arg2_len = 0;
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


void init_client_dir(session_ptr session) {
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
    memset(session->dir->mails,0,sizeof(session->dir->mails) * session->dir->mails_count);
}