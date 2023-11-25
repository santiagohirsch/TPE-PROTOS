#include "server_ADT.h"
#include <string.h>
#include "server_utils.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "./utils/logger.h"
#include "./udp/udp_ADT.h"
#define BLOCK 5
#define IPV4_PORT 1110
#define IPV6_PORT 9090
#define MAX_USERS 500

typedef struct user_node {
    session_ptr session;
    struct user_node * next;
} user_node;

struct server {
    int ipv4_socket;
    int ipv6_socket;

    int user_count;
    struct user_dir ** users_dirs;

    char * root_dir;

    user_node * users;
    int user_session_count;

    int max_concurrent_users;
    long transferred_bytes_count;
    int total_user_session_count;
    
    struct fd_handler * fd_handler;
};

struct server * server = NULL;

static int users_registered = 0;

static void init_users_dirs(char * root_dir) {
    DIR * dir = opendir(root_dir);
    if(dir == NULL) {
        log_msg(LOG_FATAL, "opendir error");
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

static void register_admin(int argc, char *argv[]) {
    if (argc == 0) {
        log_msg(LOG_FATAL, "-a: Usage: -a <username:password>");
    }
    char * username = strtok(argv[0], ":");
    if (username == NULL) {
        log_msg(LOG_FATAL, "-a: Usage: -a <username:password>");
    }
    if (strlen(username) > 15) {
        log_msg(LOG_FATAL, "-a: username too long");
    }
    char * password = strtok(NULL, ":");
    if (password == NULL) {
        log_msg(LOG_FATAL, "-a: Usage: -a <username:password>");
    }
    if (strlen(password) > 15) {
        log_msg(LOG_FATAL, "-a: password too long");
    }
    set_admin(username, password);
}

static int register_ipv4_port(int argc, char *argv[]) {
    if (argc == 0) {
        log_msg(LOG_FATAL, "-p: Usage: -p <port>");
    }
    char * port = argv[0];
    if (strlen(port) > 5) {
        log_msg(LOG_FATAL, "-p: port too long");
    }
    int port_int = atoi(port);
    if (port_int < 0 || port_int > 65535) {
        log_msg(LOG_FATAL, "-p: invalid port");
    }
    return port_int;
}

static int register_ipv6_port(int argc, char *argv[]) {
    if (argc == 0) {
        log_msg(LOG_FATAL, "-P: Usage: -P <port>");
    }
    char * port = argv[0];
    if (strlen(port) > 5) {
        log_msg(LOG_FATAL, "-P: port too long");
    }
    int port_int = atoi(port);
    if (port_int < 0 || port_int > 65535) {
        log_msg(LOG_FATAL, "-P: invalid port");
    }
    return port_int;
}

static int handle_user_option(int argc, char * argv[]) {
    if(argc == 0) {
        log_msg(LOG_ERROR, "user option requires an argument");
        close_server();
        return -1;
    }

    const char * delimeter = ":";
    char * username = strtok(argv[0], delimeter);
    if(username == NULL) {
        log_msg(LOG_ERROR, "user option requires an argument\n");
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
        log_msg(LOG_ERROR, "no password provided\n");
        close_server();
        return -1;
    }
    if(strlen(password) > 15) {
        log_msg(LOG_ERROR, "password too long\n");
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
    int ipv4_port = IPV4_PORT;
    int ipv6_port = IPV6_PORT;
    if(argc <= 1) {
        log_msg(LOG_FATAL, "usage: ./main -d <root_dir> -u <user:pass> [-u <user:pass>]...\n");
        return NULL;
    }

    server = calloc(1, sizeof(struct server));

    server->users = NULL;
    server->user_session_count = 0;
    server->total_user_session_count = 0;
    server->max_concurrent_users = MAX_USERS;
    server->fd_handler = malloc(sizeof(fd_handler));
    server->fd_handler->handle_close = close_server;

    bool dir_set = false;
    bool admin_set = false;
    bool ipv4_port_set = false;
    bool ipv6_port_set = false;
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
                log_msg(LOG_FATAL, "user option requires root dir to be set");
            }
            argv++;
            argc--;
            handle_user_option(argc,argv);
        } else if(strcmp(argv[0], "-a") == 0) {
            if (!dir_set) {
                log_msg(LOG_FATAL, "admin option requires root dir to be set");
            }

            if (admin_set) {
                log_msg(LOG_FATAL, "admin already set");
            }

            argv++;
            argc--;
            register_admin(argc, argv);
            
        } else if(strcmp(argv[0], "-p") == 0) {
            if (ipv4_port_set) {
                log_msg(LOG_FATAL, "port already set");
            }
            argv++;
            argc--;
            ipv4_port = register_ipv4_port(argc, argv);
            ipv4_port_set = true;
        }  else if(strcmp(argv[0], "-P") == 0) {
            if (ipv6_port_set) {
                log_msg(LOG_FATAL, "port already set");
            }
            argv++;
            argc--;
            ipv6_port = register_ipv6_port(argc, argv);
            ipv6_port_set = true;
        }
        else {
            fprintf(stderr, "invalid command\n");
            break;
        }
        argv++;
        argc--;
    }

    int ipv4_socket = setup_ipv4_server(ipv4_port);
    int ipv6_socket = setup_ipv6_server(ipv6_port);

    if (ipv4_socket < 0 || ipv6_socket < 0) {
        log_msg(LOG_FATAL, "setup server error");
        return NULL;
    }

    server->ipv4_socket = ipv4_socket;
    server->ipv6_socket = ipv6_socket;

    if (users_registered < server->user_count) {
        fprintf(stderr, "not all users registered\n");
        close_server();
        return NULL;
    }
    return server;
}

int get_server_ipv4_socket() {
    return server->ipv4_socket;
}

int get_server_ipv6_socket() {
    return server->ipv6_socket;
}

struct user_dir * get_user_dir(char * username, int len) {
    for(int i = 0; i < server->user_count; i++) {
        if(strncmp(server->users_dirs[i]->username, username,len) == 0 && !server->users_dirs[i]->deleted) {

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
    if(server == NULL) {
        return;
    }
    close(server->ipv4_socket);
    close(server->ipv6_socket);
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

unsigned long get_transferred_bytes_count() {
    if (server == NULL) {
        return 0;
    }
    return server->transferred_bytes_count;
}

void add_transferred_bytes_count(unsigned long bytes) {
    if (server == NULL) {
        return;
    }
    server->transferred_bytes_count += bytes;
}

unsigned int get_total_user_session_count() {
    if (server == NULL) {
        return 0;
    }
    return server->total_user_session_count;
}

unsigned int get_user_session_count() {
    if (server == NULL) {
        return 0;
    }
    return server->user_session_count;
}

int add_user(session_ptr session) {
    user_node * current = server->users;
    if (current == NULL) {
        current = malloc(sizeof(user_node));
        current->session = session;
        current->next = NULL;
        server->users = current;
        server->user_session_count++;
        server->total_user_session_count++;
        return 0;
    }
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(user_node));
    current->next->session = session;
    current->next->next = NULL;
    server->user_session_count++;
    server->total_user_session_count++;
    return 0;
}

int remove_user(session_ptr session) {
    if (server == NULL)
    {
        return 0;
    }

    user_node * prev = server->users;

    if (prev == NULL)
    {
        return -1;
    }

    user_node * current = prev->next;

    if (current == NULL && prev->session == session) {
        server->users = NULL;
        server->user_session_count--;
        free(prev);
        return 0;
    }

    while (current != NULL) {
        if (current->session == session) {
            user_node * to_free = current;
            prev->next = current->next;
            free(to_free);
            server->user_session_count--;
            return 0;
        }
        prev = current;
        current = prev->next;
    }
    return -1;
    
}

int set_max_concurrent_users(int max) {
    if (server->max_concurrent_users > max) {
        return -1;
    }
    server->max_concurrent_users = max;
    return 0;
}

int server_full() {
    return server->user_session_count == server->max_concurrent_users;
}

static int get_file_count(DIR * dir) {
    int count = 0;
    struct dirent * entry;
    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            count++;
        }
    }
    return count;
}

static void delete_files(DIR * dir, char * path) {
    struct dirent * entry;
    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char file_path[MAX_MAIL_PATH_LEN] = {0};
            strcpy(file_path, path);
            strcat(file_path, "/");
            strncat(file_path, entry->d_name, strlen(entry->d_name));
            if (remove(file_path) == 0) {
                log_msg(LOG_INFO, "Deleted file: %s", file_path);
            }
        }
    }
}

int delete_user_dir(char * username, int len) {
    char *path = get_root_dir();
    DIR * dir = opendir(path);
    int ret = 0;

    struct dirent * entry;
    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, username) == 0) {
                char user_path[MAX_MAIL_PATH_LEN] = {0};
                strcpy(user_path, path);
                strcat(user_path, "/");
                strncat(user_path, username, len);
                DIR * user_dir = opendir(user_path);
                if (user_dir != NULL) {
                    delete_files(user_dir, user_path);
                    if (rmdir(user_path) == 0) {
                        log_msg(LOG_INFO, "Deleted directory: %s", user_path);
                    }

                    if (get_file_count(user_dir) == 0) {
                        ret = 0;
                    } else {
                        ret = 1;
                    }
                    closedir(user_dir);
                }
                break;
            }
        }
    }
    closedir(dir);
    return ret;
}