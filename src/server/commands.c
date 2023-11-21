#include "commands.h"
#include "server_ADT.h"
#include "../session/session.h"
#include <string.h>

int user_cmd(session_ptr session, char *arg, int arg_len, char *response) {

    int len = strlen("+OK\n");

    strncpy(response, "+OK\n", len);
    set_username(session, arg, arg_len);

    return len;
}

// TODO: Check password
void pass_cmd(session_ptr session, char *arg, int arg_len, char *response) {

    char *username = malloc(USERNAME_MAX_LEN);
    int username_len = get_username(session, username);

    if (username_len == 0) {
        int len = strlen("-ERR [AUTH] Authentication failed\n");
        strncpy(response, "-ERR [AUTH] Authentication failed\n", len);
        return;
    }
    int len = strlen("+OK\n");

    strncpy(response, "+OK\n", len);

    char dir[256] = {0};

    strcpy(dir, get_root_dir());

    strcat(dir, "/");

    strcat(dir, username);

    set_dir(session, opendir(dir));
}