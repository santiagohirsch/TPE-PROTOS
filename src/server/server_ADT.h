#ifndef _SERVER_ADT_H_
#define _SERVER_ADT_H_

#include "../selector/selector.h"
#include "../session/session.h"

typedef struct server *server_t;
typedef struct user_dir *user_dir_t;


server_t init_server(char * root_dir, int port);

user_dir_t get_user_dir(char * username);

int get_server_socket();

void close_server();

char * get_root_dir();

struct fd_handler * get_fd_handler();

void set_fd_handler(void (*handle_read)(struct selector_key * key), void (*handle_write)(struct selector_key * key));

int add_user(session_ptr session);

#endif