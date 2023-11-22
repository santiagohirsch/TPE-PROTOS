#include "commands.h"
#include "server_ADT.h"
#include "../session/session.h"
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int user_cmd(session_ptr session, char *arg, int arg_len, char *response) {

    pop_action(session);

    int len = strlen("+OK\n");

    strncpy(response, "+OK\n", len);
    set_username(session, arg, arg_len);

    return len;
}

// TODO: Check password
int pass_cmd(session_ptr session, char *arg, int arg_len, char *response, bool *is_authenticated) {

    pop_action(session);

    int len = 0;
    char username[USERNAME_MAX_LEN] = {0};
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

    strncat(dir, username, username_len);

    // TODO: chequeo de error
    DIR *dir_ptr = opendir(dir);

    set_dir(session, dir_ptr);

    *is_authenticated = true;

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

static void get_file_stats(DIR * dir, const char * path, int * file_count, int * bytes) {
    struct dirent * entry;

   while((entry = readdir(dir)) != NULL) {
       if(entry->d_type == DT_REG) {
           *file_count += 1;
           *bytes += get_file_size(path, entry->d_name);
       }
   }
}

int stat_cmd(session_ptr session, char * arg, int len, char * response) {
   
   pop_action(session);
   
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
                bytes += get_file_size(mail_dir, entry->d_name);
            }
       }
       i++;
   }

   get_file_stats(dir, mail_dir, &file_count, &bytes);

   sprintf(response, "+OK %d %d\n", file_count, bytes);

   return strlen(response);
}

int dele_cmd(session_ptr session, char * arg, int len, char * response) {
    
    pop_action(session);
    
    int status = mark_to_delete(session, atoi(arg));
    if (status < 0) {
        strcpy(response, "-ERR DELE: Maybe no such message\r\n");
        return -1;
    }
    return 0;
}

int rset_cmd(session_ptr session, char * arg, int len, char * response) {
    
    pop_action(session);
    
    reset_marks(session);
    return 0;
}

static struct dirent * read_files(DIR * dir, long msg_num) {
    struct dirent * entry = readdir(dir);
    int i = 0;
    while(i < msg_num && entry != NULL) {
        
        if(entry->d_type == DT_REG) {
            i++;
        }
        entry = readdir(dir);
    }
    return entry;
}


int list_cmd(session_ptr session, char * arg, int len, char * response, int bytes) {
    
    action_type action = pop_action(session);

    DIR * dir = get_dir(session);
    long msg_num;

    struct dirent * entry;

    char username[USERNAME_MAX_LEN] = {0};
    char path[256] = {0};
    int username_len = get_username(session, username);
    strcpy(path, get_root_dir());
    strcat(path, "/");
    strncat(path, username, username_len);
    strcat(path, "/");
    int path_len = strlen(path);

    struct stat st;

    if (len > 1) {
        msg_num = strtol(arg, NULL, 10);
        rewinddir(dir);
        entry = read_files(dir, msg_num);

        if (entry == NULL || msg_num < 1) {
            return sprintf(response, "-ERR There's no message %ld.\r\n", msg_num);
        }

        strcat(path, entry->d_name);
        stat(path, &st);
        return sprintf(response, "+OK %ld %lld\r\n", msg_num, st.st_size);
    }

    int response_len = 0;
    int current_line_len = 0;
    char aux[256] = {0};


    if (action == PROCESS) {
        rewinddir(dir);
        int count = 0;
        int bytes = 0;
        get_file_stats(dir, path, &count, &bytes);
        current_line_len = sprintf(aux, "+OK %d messages (%d octets)\r\n", count, bytes);
        response_len += current_line_len;
        strncpy(response, aux, current_line_len);
        response[response_len] = '\0';
        rewinddir(dir);
        set_user_dir_idx(session, 1);
    }
    

    int idx = get_user_dir_idx(session);
    entry = readdir(dir);
    long location = telldir(dir);

    while (response_len + current_line_len < bytes && entry != NULL) {
        if (entry->d_type == DT_REG) {
            path[path_len] = '\0';
            strcat(path, entry->d_name);
            stat(path, &st);
            current_line_len = sprintf(aux, "%d %lld\r\n", idx, st.st_size);
            if (response_len + current_line_len < bytes) {
                response_len += current_line_len;
                strncat(response, aux, current_line_len);
                idx++;
            }
            location = telldir(dir);
        }
        
        entry = readdir(dir);
    }

    if (entry != NULL) {
        seekdir(dir, location);
        set_dir(session, dir);
        set_user_dir_idx(session, idx);
        push_action(session, PROCESSING);
        return response_len;
    }

    current_line_len = sprintf(aux, ".\r\n");
    if (response_len + current_line_len < bytes) {
        response_len += current_line_len;
        strncat(response, aux, current_line_len);
    } else {
        push_action(session, PROCESSING);
    }

    return response_len;
}