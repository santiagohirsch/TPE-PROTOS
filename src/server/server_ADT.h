#ifndef _SERVER_ADT_H_
#define _SERVER_ADT_H_

#include "./selector/selector.h"
#include "./session/session.h"

typedef struct server *server_t;

struct user_dir {
    char username[USERNAME_MAX_LEN];
    char pass[16];
    bool is_open;
};

typedef struct user_admin {
    char username[USERNAME_MAX_LEN];
    char pass[16];
} user_admin;

user_admin * get_admin();

int get_server_ipv4_socket();

int get_server_ipv6_socket();

server_t init_server(int argc, char * argv[]);

void close_server();

struct user_dir * get_user_dir(char * username, int len);

char * get_root_dir();

struct fd_handler * get_fd_handler();

void set_fd_handler(void (*handle_read)(struct selector_key * key), void (*handle_write)(struct selector_key * key));

unsigned long get_transferred_bytes_count();

void add_transferred_bytes_count(unsigned long bytes);

unsigned int get_user_session_count();

unsigned int get_total_user_session_count();

int add_user(session_ptr session);

int remove_user(session_ptr session);

#endif