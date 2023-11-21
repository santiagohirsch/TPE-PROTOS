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
int pass_cmd(session_ptr session, char *arg, int arg_len, char *response, bool *is_authenticated) {

    int len = 0;
    char *username = malloc(USERNAME_MAX_LEN);
    int username_len = get_username(session, username);

    if (username_len == 0) {
        len = strlen("-ERR [AUTH] Authentication failed\n");
        strncpy(response, "-ERR [AUTH] Authentication failed\n", len);
        *is_authenticated = false;
        return len;
    }
    len = strlen("+OK\n");

    strncpy(response, "+OK\n", len);

    char dir[256] = {0};

    strcpy(dir, get_root_dir());

    strcat(dir, "/");

    strcat(dir, username);

    DIR *dir_ptr = opendir(dir);

    set_dir(session, dir_ptr);

    *is_authenticated = true;

    closedir(dir_ptr);

    return len;
}