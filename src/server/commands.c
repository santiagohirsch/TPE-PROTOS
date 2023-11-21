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

static ssize_t get_file_size(const char * mail, const char * name) {
    struct stat st;
    char * path = malloc((strlen(mail) + strlen(name) + 1) * sizeof(char));

    strcpy(path, mail);
    strcat(path, "/");
    strncat(path, name, strlen(name));

    if(stat(path, &st) < 0) {
        free(path);
        perror("stat error");
        exit(1); //TODO error code
    }
    free(path);

    return st.st_size;
}

int stat_cmd(session_ptr session, char * arg, int len, char * response) {
   DIR * dir = get_dir(session);

   if(!dir) {
        perror("opendir error");
        exit(1); //TODO error code
   }

   char username[USERNAME_MAX_LEN] = {0};
   int username_len = get_username(session, username);
   if (username_len < 0) {
         perror("get username error");
         exit(1); //TODO error code
   } 

   char mail_dir[256] = {0};
   strcopy(mail_dir, get_root_dir());
   strcat(mail_dir, "/");
   strncat(mail_dir, username, username_len);

   int file_count = 0;
   int bytes = 0;

   struct dirent * entry;

   while((entry = readdir(dir)) != NULL) {
       if(entry->d_type == DT_REG) {
           file_count++;
           bytes += get_file_size(mail_dir, entry->d_name);
       }
   }

   sprintf(response, "+OK %d %d\n", file_count, bytes);

   return strlen(response);
}

int dele_cmd(session_ptr session, char * arg, int len, char * response) {
    int status = mark_to_delete(session, atoi(arg));
    if (status < 0) {
        strcopy(response, "-ERR DELE: Maybe no such message\r\n")
        return -1;
    }
    return 0;
}

int rset_cmd(session_ptr session, char * arg, int len, char * response) {
    reset_marks(session);
    return 0;
}