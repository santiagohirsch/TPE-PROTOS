#ifndef _SERVER_ADT_H_
#define _SERVER_ADT_H_

#include "../selector/selector.h"
#include "../session/session.h"

typedef struct server *server_t;

struct user_dir {
    char username[USERNAME_MAX_LEN];
    char pass[16];
    bool is_open;
};


server_t init_server(int argc, char * argv[]);

struct user_dir * get_user_dir(char * username, int len);

int get_server_socket();

void close_server();

char * get_root_dir();

struct fd_handler * get_fd_handler();

void set_fd_handler(void (*handle_read)(struct selector_key * key), void (*handle_write)(struct selector_key * key));

int add_user(session_ptr session);

int remove_user(session_ptr session);

#endif