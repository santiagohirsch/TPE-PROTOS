#include "server_ADT.h"
#include <string.h>
#include "server_utils.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define BLOCK 5
#define PORT 1110

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
    int user_session_count;
    struct fd_handler * fd_handler;
};

struct server * server = NULL;

static int users_registered = 0;

static void init_users_dirs(char * root_dir) {
    DIR * dir = opendir(root_dir);
    if(dir == NULL) {
        perror("opendir error");
        exit(1);
    }

    server->root_dir = root_dir;

    int count = 0;
    struct dirent * entry;
    struct user_dir ** users = calloc(BLOCK, sizeof(struct user_dir *));

    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_DIR) {
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                if (count > 0 && ((count % BLOCK) == 0)) {
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

    server->users_dirs = users;
    server->user_count = count;
}

static int handle_user_option(int argc, char * argv[]) {
    if(argc == 0) {
        fprintf(stderr, "user option requires an argument\n");
        close_server();
        return -1;
    }

    const char * delimeter = ":";
    char * username = strtok(argv[0], delimeter);
    if(username == NULL) {
        fprintf(stderr, "user option requires an argument\n");
        close_server();
        return -1;
    }

    int idx = -1;
    for(int i = 0; i < server->user_count; i++) {
        if(strcmp(argv[0], server->users_dirs[i]->username) == 0) {
            idx = i;
        }
    }
    if(idx == -1) {
        return -1;
    }
    char * password = strtok(NULL, delimeter);
    if(password == NULL) {
        fprintf(stderr, "no password provided\n");
        close_server();
        return -1;
    }
    if(strlen(password) > 15) {
        fprintf(stderr, "password too long\n");
        close_server();
        return -1;
    }
    strcpy(server->users_dirs[idx]->pass, password);
    users_registered++;
    return 1;
}

struct server * init_server(int argc, char * argv[]) {
    if(server != NULL) {
        return server;
    }

    if(argc <= 1) {
        fprintf(stderr, "no root dir and users provided\n");
        fprintf(stderr, "usage: ./main -d <root_dir> -u <user:pass> [-u <user:pass>]...\n");
        return NULL;
    }
    
    int server_socket = setup_server(PORT);
    if(server_socket < 0) {
        perror("setup server error");
        return NULL;
    }

    server = calloc(1, sizeof(struct server));
    server->socket = server_socket;

    server->users = NULL;
    server->user_session_count = 0;
    server->fd_handler = malloc(sizeof(fd_handler));

    bool dir_set = false;

    argv++;
    argc--;
    while(argc > 0) {

        if(strcmp(argv[0], "-d") == 0) {
            if (dir_set) {
                fprintf(stderr, "root dir already set\n");
                close_server();
                return NULL;
            } else {
                argv++;
                argc--;
                if(argc == 0) {
                    fprintf(stderr, "root dir option requires an argument\n");
                    close_server();
                    return NULL;
                }
                server->root_dir = argv[0];
                init_users_dirs(argv[0]);
                dir_set = true;
            }
        } else if(strcmp(argv[0],"-u") == 0) {
            if (!dir_set) {
                fprintf(stderr, "root dir not set\n");
                close_server();
                return NULL;
            }
            argv++;
            argc--;
            handle_user_option(argc,argv);
        }
        else {
            fprintf(stderr, "invalid command\n");
            break;
        }
        argv++;
        argc--;
    }

    if (users_registered < server->user_count) {
        fprintf(stderr, "not all users registered\n");
        close_server();
        return NULL;
    }
    return server;
}

int get_server_socket() {
    return server->socket;
}

struct user_dir * get_user_dir(char * username, int len) {
    for(int i = 0; i < server->user_count; i++) {
        if(strncmp(server->users_dirs[i]->username, username,len) == 0) {

            return server->users_dirs[i];
        }
    }
    return NULL;
}

static void free_users() {
    user_node * current = server->users;
    while(current != NULL) {
        user_node * next = current->next;
        delete_user_session(current->session);
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
    if (server->users_dirs != NULL) {
        free_users_dirs();
    }
    free(server->fd_handler);
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
        server->user_session_count++;
        return 0;
    }
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(user_node));
    current->next->session = session;
    current->next->next = NULL;
    server->user_session_count++;
    return 0;
}