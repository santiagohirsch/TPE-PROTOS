#include "server_ADT.h"
#include <string.h>
#include "server_utils.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BLOCK 5

struct user_dir {
    char * username;
    char * pass;
};

typedef struct user_node {
    session_ptr session;
    struct user_node * next;
} user_node;

struct server {
    int socket;
    int user_count;
    char * root_dir;
    struct user_dir ** users_dirs;
    user_node * users;
    struct fd_handler * fd_handler;
};

struct server * server = NULL;

static struct user_dir ** init_users_dirs(char * root_dir, int * user_count) {
    DIR * dir = opendir(root_dir);
    if(dir == NULL) {
        perror("opendir error");
        exit(1);
    }

    int count = 0;
    struct dirent * entry;
    struct user_dir ** users = calloc(BLOCK, sizeof(struct user_dir *));

    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_DIR) {
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                if (count > 0 && (count % BLOCK == 0)) {
                    users = realloc(users, sizeof(struct user_dir *) * (count + BLOCK));
                    memset(users + count, 0, sizeof(struct user_dir *) * BLOCK);
                }
                users[count] = calloc(1, sizeof(struct user_dir));
                strncpy(users[count]->username, entry->d_name, strlen(entry->d_name));
                count++;
            }
        }
    }
    users[count] = NULL;

    closedir(dir);
    *user_count = count;
    return users;
}

struct server * init_server(char * root_dir, int port) {
    if(server != NULL) {
        return server;
    }
    
    int server_socket = setup_server(port);
    if(server_socket < 0) {
        perror("setup server error");
        return NULL;
    }

    server = malloc(sizeof(struct server));
    server->socket = server_socket;
    server->root_dir = root_dir;
    server->users_dirs = init_users_dirs(root_dir, &server->user_count);
    server->users = NULL;
    server->user_count = 0;
    server->fd_handler = malloc(sizeof(struct fd_handler));
    return server;
}

int get_server_socket() {
    return server->socket;
}

user_dir_t get_user_dir(char * username) {
    for(int i = 0; i < server->user_count; i++) {
        if(strcmp(server->users_dirs[i]->username, username) == 0) {
            return server->users_dirs[i];
        }
    }
    return NULL;
}

static void free_users() {
    user_node * current = server->users;
    while(current != NULL) {
        user_node * next = current->next;
        free(current);
        current = next;
    }
}

static void free_users_dirs() {
    struct user_dir ** user_dirs = server->users_dirs;
    for(int i = 0; i < server->user_count; i++) {
        free(user_dirs[i]);
    }
    free(user_dirs);
}

void close_server() {
    close(server->socket);
    free_users();
    free_users_dirs();
    free(server);
    server = NULL;
}

char * get_root_dir() {
    return server->root_dir;
}

struct fd_handler * get_fd_handler() {
    return server->fd_handler;
}

void set_fd_handler(void (*handle_read)(struct selector_key * key), void (*handle_write)(struct selector_key * key)) {
    server->fd_handler->handle_read = handle_read;
    server->fd_handler->handle_write = handle_write;
}

int add_user(session_ptr session) {
    user_node * current = server->users;
    if (current == NULL) {
        current = malloc(sizeof(user_node));
        current->session = session;
        current->next = NULL;
        server->users = current;
        server->user_count++;
        return 0;
    }
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(user_node));
    current->next->session = session;
    current->next->next = NULL;
    server->user_count++;
    return 0;
}