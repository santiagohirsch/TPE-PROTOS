#include "user_request.h"  
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>

struct request {
    char username[32];
    char password[32];
    int id;
    char command[32];
    char arg1[32];
    char arg2[32];
}request;

typedef int(*request_handler)(struct request *req, int argc, char *argv[]);

struct option {
    char option[32];
    request_handler handler;
}option;

static void help_handler()
{
    printf("Usage: ./pop3d-user -a <user>:<pass> COMMAND [ARGUMENT]...\n");
    printf("Commands:\n");
    printf("\tcurrent\n");
    printf("\thistory\n");
    printf("\tbytes\n");
    printf("\tpassword   <user> <password>\n");
    printf("\tdelete     <user>\n");
    printf("\tconcurrent <max_concurrent_users>\n");
}

static int auth_handler(struct request *req, int argc, char *argv[])
{
    if (argc < 1) {
        fprintf(stderr, "Missing username or password\n");
        fprintf(stderr, "Usage: ./user -a <user:pass>\n");
        exit(1);
    }

    char *user = strtok(argv[0], ":");
    if (user == NULL) {
        fprintf(stderr, "Missing username\n");
        fprintf(stderr, "Usage: ./user -a <user:pass>\n");
        exit(1);
    }

    char *pass = strtok(NULL, ":");
    if (pass == NULL) {
        fprintf(stderr, "Missing password\n");
        fprintf(stderr, "Usage: ./user -a <user:pass>\n");
        exit(1);
    }

    if (strlen(user) > 32) {
        fprintf(stderr, "Username too long\n");
        exit(1);
    }

    if (strlen(pass) > 32) {
        fprintf(stderr, "Password too long\n");
        exit(1);
    }

    strcpy(req->username, user);
    strcpy(req->password, pass);

    return 1;
}

static int current_handler(struct request *req, int argc, char *argv[])
{
    if (argc != 0) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Too many arguments\n");
        fprintf(stderr, "Usage: ./user current\n");
        exit(1);
    }
    strcpy(req->command, "current");
    return 1;
}

static int history_handler(struct request *req, int argc, char *argv[])
{
    if (argc != 0) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Too many arguments\n");
        fprintf(stderr, "Usage: ./user history\n");
        exit(1);
    }
    strcpy(req->command, "history");
    return 1;
}

static int bytes_handler(struct request *req, int argc, char *argv[])
{
    if (argc != 0) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Too many arguments\n");
        fprintf(stderr, "Usage: ./user bytes\n");
        exit(1);
    }
    strcpy(req->command, "bytes");
    return 1;
}

static int password_handler(struct request *req, int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Missing arguments\n");
        fprintf(stderr, "Usage: ./user password <user password>\n");
        exit(1);
    }

    if (argc > 2) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Too many arguments\n");
        fprintf(stderr, "Usage: ./user password <user password>\n");
        exit(1);
    }

    if (strlen(argv[1]) > 32) {
        fprintf(stderr, "Password too long\n");
        exit(1);
    }

    strcpy(req->command, "password");
    strcpy(req->arg1, argv[0]);
    strcpy(req->arg2, argv[1]);
    return 1;
}

static int delete_handler(struct request *req, int argc, char *argv[])
{
    if (argc < 1) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Missing arguments\n");
        fprintf(stderr, "Usage: ./user delete <user>\n");
        exit(1);
    }

    if (argc > 1) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Too many arguments\n");
        fprintf(stderr, "Usage: ./user delete <user>\n");
        exit(1);
    }

    if (strlen(argv[0]) > 32) {
        fprintf(stderr, "Username too long\n");
        exit(1);
    }

    strcpy(req->command, "delete");
    strcpy(req->arg1, argv[0]);
    return 1;
}

static int concurrent_handler(struct request *req, int argc, char *argv[])
{
    if (argc < 1) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Missing arguments\n");
        fprintf(stderr, "Usage: ./user concurrent <max_users>\n");
        exit(1);
    }

    if (argc > 1) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Too many arguments\n");
        fprintf(stderr, "Usage: ./user concurrent <max_users>\n");
        exit(1);
    }

    int max_users = atoi(argv[0]);
    if (max_users < 1 || max_users > 1000) {
        fprintf(stderr, "Invalid argument\n");
        fprintf(stderr, "Max users must be between 1 and 1000\n");
        exit(1);
    }

    strcpy(req->command, "concurrent");
    strcpy(req->arg1, argv[0]);
    return 1;
}

struct option options[] = {
    {"current", current_handler},
    {"history", history_handler},
    {"bytes", bytes_handler},
    {"password", password_handler},
    {"delete", delete_handler},
    {"concurrent", concurrent_handler}
};

struct request * get_request(int argc, char * argv[]) {
    if (argc == 0) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Usage: ./user -a <user:pass> COMMAND [ARGUMENT]...\n");
        fprintf(stderr, "Try './user -h' for more information.\n");
        exit(1);
    }
    struct request *req = NULL;
    if (strcmp(argv[0], "-a") == 0) {
        argc--;
        argv++;
        req = calloc(1, sizeof(struct request));
        int args = auth_handler(req, argc, argv);
        argc -= args;
        argv += args;
    } else if (strcmp(argv[0], "-h") == 0 && argc == 1) {
        help_handler();
        exit(0);
    } else {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "First argument must be '-a' or '-h'\n");
        fprintf(stderr, "Try './user -h' for more information.\n");
        exit(1);
    }

    if (argc == 0) {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Missing command\n");
        fprintf(stderr, "Usage: ./user -a <user:pass> COMMAND [ARGUMENT]...\n");
        fprintf(stderr, "Try './user -h' for more information.\n");
        exit(1);
    }


    bool found = false;
    request_handler handler;
    for (long unsigned int i = 0; i < sizeof(options) / sizeof(struct option) && argc > 0 && !found; i++) {
        if (strcmp(argv[0], options[i].option) == 0) {
            found = true;
            handler = options[i].handler;
        }
    }

    if (!found) {
        free(req);
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Invalid command\n");
        fprintf(stderr, "Usage: ./user -a <user:pass> COMMAND [ARGUMENT]...\n");
        fprintf(stderr, "Try './user -h' for more information.\n");
        exit(1);
    } else {
        argc--;
        argv++;
        handler(req, argc, argv);
    }

    req->id = 1;
    return req;
}

void build_request(struct request * req, char * buf) {
    strcpy(buf, "user protocol\r\n");
    strcat(buf, "username: ");
    strcat(buf, req->username);
    strcat(buf, "\r\n");
    strcat(buf, "password: ");
    strcat(buf, req->password);
    strcat(buf, "\r\n");
    strcat(buf, "id: ");
    char id[4];
    sprintf(id, "%d\r\n", req->id);
    strcat(buf, id);
    strcat(buf, "command: ");
    strcat(buf, req->command);
    strcat(buf, "\r\n");
    if (req->arg1[0] != '\0') {
        strcat(buf, "arg1: ");
        strcat(buf, req->arg1);
        strcat(buf, "\r\n");
    }
    if (req->arg2[0] != '\0') {
        strcat(buf, "arg2: ");
        strcat(buf, req->arg2);
        strcat(buf, "\r\n");
    }
    strcat(buf, ".\r\n");
}
