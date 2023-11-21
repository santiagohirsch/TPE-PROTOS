#include "commands.h"
#include "server_ADT.h"
#include "../session/session.h"
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

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

    if (username_len <= 0) {
        len = strlen("-ERR [AUTH] Authentication failed\r\n");
        strncpy(response, "-ERR [AUTH] Authentication failed\r\n", len);
        *is_authenticated = false;
        return len;
    }

    struct user_dir * user_dir = get_user_dir(username, username_len);

    if (strcmp(user_dir->pass, arg) != 0) {
        len = strlen("-ERR [AUTH] Authentication failed\r\n");
        strncpy(response, "-ERR [AUTH] Authentication failed\r\n", len);
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

static ssize_t get_file_size(const char * mail, const char * file) {
    struct stat st;
    int mail_len = strlen(mail);
    int file_len = strlen(file);
    char * path = (char *) calloc(mail_len + file_len + 2, sizeof(char));
    strncpy(path, mail, mail_len);
    strcat(path, "/");
    strncat(path, file, file_len + 1);

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
   strcpy(mail_dir, get_root_dir());
   strcat(mail_dir, "/");
   strncat(mail_dir, username, username_len);

   int file_count = 0;
   int bytes = 0;

   struct dirent * entry;

   int * user_mails = get_dir_mails(session);
   int i = 0;
   while((entry = readdir(dir)) != NULL) {
       if(entry->d_type == DT_REG) {
           if(!user_mails[i]) {
            file_count++;
           bytes += get_file_size(mail_dir, entry->d_name);}
       }
       i++;
   }

   sprintf(response, "+OK %d %d\n", file_count, bytes);

   return strlen(response);
}

int dele_cmd(session_ptr session, char * arg, int len, char * response) {
    int status = mark_to_delete(session, atoi(arg));
    if (status < 0) {
        strcpy(response, "-ERR DELE: Maybe no such message\r\n");
        return -1;
    }
    return 0;
}

int rset_cmd(session_ptr session, char * arg, int len, char * response) {
    reset_marks(session);
    return 0;
}