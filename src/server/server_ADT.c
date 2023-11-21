#include "server_ADT.h"
#include <string.h>
#include "server_utils.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

#define BLOCK 5

struct user_dir {
    char * username;
    char * pass;
};

struct server {
    int socket;
    int user_count;
    char * root_dir;
    struct user_dir ** users;
};

struct server * server = NULL;

static struct user_dir ** init_users(char * root_dir, int * user_count) {
    DIR * dir = opendir(root_dir);
    if(dir == NULL) {
        perror("opendir error");
        exit(1);
    }

    int count = 0;
    struct dirent * entry;
    struct user_dir ** users = malloc(sizeof(struct user_dir *));

    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_DIR) {
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                if (count % BLOCK == 0) {
                    users = realloc(users, sizeof(struct user_dir *) * (count + BLOCK));
                }
                users[count] = calloc(1,sizeof(struct user_dir));
                strncpy(users[count]->username, entry->d_name, strlen(entry->d_name));
                count++;
            }
        }
    }

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
    server->users = init_users(root_dir, &server->user_count);
    return server;
}

int get_server_socket() {
    return server->socket;
}

user_dir_t get_user_dir(char * username) {
    for(int i = 0; i < server->user_count; i++) {
        if(strcmp(server->users[i]->username, username) == 0) {
            return server->users[i];
        }
    }
    return NULL;
}

void close_server() {
    for(int i = 0; i < server->user_count; i++) {
        free(server->users[i]);
    }
    free(server->users);
    free(server);
    server = NULL;
}

char * get_root_dir() {
    return server->root_dir;
}