#include "commands.h"
#include "server_ADT.h"
#include "./session/session.h"
#include "pop3_constants.h"
#include "./utils/logger.h"

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>


int user_cmd(session_ptr session, char *arg, int arg_len, char *response) {
    log_msg(LOG_INFO, "user command");

    pop_action(session);

    struct user_dir * user_dir = get_user_dir(arg, arg_len);

    int len = strlen("+OK\n");

    if (user_dir == NULL) {
        log_msg(LOG_INFO, "Invalid username");

        return len;
    }


    strncpy(response, "+OK\n", len);
    set_username(session, arg, arg_len);

    log_msg(LOG_INFO, "User %s set", arg);

    return len;
}

// TODO: Check password
int pass_cmd(session_ptr session, char *arg, int arg_len, char *response, bool *is_authenticated) {
    log_msg(LOG_INFO, "pass command");

    action_type action = pop_action(session);
    log_msg(LOG_INFO, "action: %d", action);

    int len = 0;
    char username[USERNAME_MAX_LEN] = {0};
    int username_len = get_username(session, username);
   
    if (username_len <= 0) {
        log_msg(LOG_INFO, "Invalid username");   // TODO: CHECK if this is the correct error message or is vulnerable

        len = strlen("-ERR [AUTH] Authentication failed\r\n");
        strncpy(response, "-ERR [AUTH] Authentication failed\r\n", len);
        *is_authenticated = false;
        return len;
    }

    struct user_dir * user_dir = get_user_dir(username, username_len);

    if (strcmp(user_dir->pass, arg) != 0) {
        log_msg(LOG_INFO, "Invalid password");  // TODO: CHECK if this is the correct error message or is vulnerable

        len = strlen("-ERR [AUTH] Authentication failed\r\n");
        strncpy(response, "-ERR [AUTH] Authentication failed\r\n", len);
        return len;
    }
    len = strlen("+OK\n");

    strncpy(response, "+OK\n", len);

    char dir[MAX_MAIL_PATH_LEN] = {0};

    strcpy(dir, get_root_dir());

    strcat(dir, "/");

    strncat(dir, username, username_len);

    strcat(dir, "/");

    strcat(dir, "cur");

    DIR *dir_ptr = opendir(dir);

    if (dir_ptr == NULL) {
        log_msg(LOG_ERROR, "pass command: opendir error");
        return -1;
    }

    set_dir(session, dir_ptr);

    *is_authenticated = true;

    log_msg(LOG_INFO, "User %s authenticated", username);
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
        log_msg(LOG_ERROR, "stat error");
        return -1;
    }
    free(path);
    return st.st_size;
}

static void get_file_stats(DIR * dir, const char * path, int * file_count, int * bytes, int * marked_mails) {
    struct dirent * entry;
    int i = 0;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_REG) {
                if(!marked_mails[i++]) {
                    *file_count += 1;
                    ssize_t size = get_file_size(path, entry->d_name);
                    if (size < 0) {
                        log_msg(LOG_ERROR, "get_file_size error");
                        return;
                    } else {
                        *bytes += size;
                    }
                }
        }
    }
}

int stat_cmd(session_ptr session, char * arg, int len, char * response) {
    log_msg(LOG_INFO, "stat command");

    action_type action = pop_action(session);
    log_msg(LOG_INFO, "action: %d", action);

    char username[USERNAME_MAX_LEN] = {0};
    int username_len = get_username(session, username);
    if (username_len < 0) {
        log_msg(LOG_ERROR, "get_username error");
        return -1;
    } 

    char mail_dir[MAX_MAIL_PATH_LEN] = {0};
    strcpy(mail_dir, get_root_dir());
    strcat(mail_dir, "/");
    strncat(mail_dir, username, username_len);
    strcat(mail_dir, "/");
    strcat(mail_dir, "cur/");

    int * user_mails = get_dir_mails(session);

    DIR * dir = get_dir(session);
    if(!dir) {
        log_msg(LOG_ERROR, "stat command: opendir error");
        return -1;
    }

    int file_count = 0;
    int bytes = 0;
    rewinddir(dir);

    get_file_stats(dir, mail_dir, &file_count, &bytes, user_mails);

    sprintf(response, "%d %d", file_count, bytes);

    return strlen(response);
}

int dele_cmd(session_ptr session, char * arg, int len, char * response) {
    log_msg(LOG_INFO, "dele command");
    
    action_type action = pop_action(session);
    log_msg(LOG_INFO, "action: %d", action);
    
    int status = mark_to_delete(session, atoi(arg));
    log_msg(LOG_INFO, "dele status: %d", status);

    if (status < 0) {
        char aux[256] = {0};
        int aux_len = sprintf(aux, "-ERR There's no message %s\r\n", arg);
        strncpy(response, aux, aux_len);
        return -1;
    }
    return 0;
}

int rset_cmd(session_ptr session, char * arg, int len, char * response) {
    log_msg(LOG_INFO, "rset command");
    
    action_type action = pop_action(session);
    log_msg(LOG_INFO, "action: %d", action);
    
    reset_marks(session);

    return 0;
}

static struct dirent * read_files(DIR * dir, long msg_num) {
    struct dirent * entry = readdir(dir);   // not ordered
    int i = 0;
    while (entry != NULL) {
        if (entry->d_type == DT_REG) {
            i++;
            if (i == msg_num) {
                return entry; // Devolver el archivo cuando se encuentre el número correspondiente
            }
        }
        entry = readdir(dir);
    }

    return NULL;
}


int list_cmd(session_ptr session, char * arg, int len, char * response, int bytes) {
    log_msg(LOG_INFO, "list command");

    action_type action = pop_action(session);
    log_msg(LOG_INFO, "action: %d", action);

    DIR * dir = get_dir(session);
    long msg_num;

    struct dirent * entry;

    char username[USERNAME_MAX_LEN] = {0};
    char path[MAX_MAIL_PATH_LEN] = {0};
    int username_len = get_username(session, username);
    strcpy(path, get_root_dir());
    strcat(path, "/");
    strncat(path, username, username_len);
    strcat(path, "/");
    strcat(path, "cur/");
    int path_len = strlen(path);
    int * user_mails = get_dir_mails(session);
    struct stat st;

    if (len > 1) {
        msg_num = strtol(arg, NULL, 10);
        rewinddir(dir);
        entry = read_files(dir, msg_num);

        if (msg_num < 1 || msg_num > get_dir_mails_count(session) || entry == NULL) {
            log_msg(LOG_INFO, "There's no message %ld.", msg_num);
            return sprintf(response, "-ERR There's no message %ld.\r\n", msg_num);
        } else if (is_marked_to_delete(session, msg_num)) {
            log_msg(LOG_INFO, "Message is deleted.");
            return sprintf(response, "-ERR Message is deleted.\r\n");
        }

        strcat(path, entry->d_name);
        stat(path, &st);
        
        return sprintf(response, "+OK %ld %ld\r\n", msg_num, st.st_size);
    }

    int response_len = 0;
    int current_line_len = 0;
    char aux[MAX_RESPONSE_LEN] = {0};


    if (action == PROCESS) {
        log_msg(LOG_INFO, "list command: process");
        rewinddir(dir);
        int count = 0;
        int bytes = 0;
        get_file_stats(dir, path, &count, &bytes, user_mails);
        response_len = sprintf(aux, "+OK %d messages (%d octets)\r\n", count, bytes);
        strncpy(response, aux, response_len);
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
            if(!is_marked_to_delete(session,idx)) {
                current_line_len = sprintf(aux, "%d %ld\r\n", idx, st.st_size);
                if (response_len + current_line_len < bytes) {
                    response_len += current_line_len;
                    strncat(response, aux, current_line_len);
                    idx++;
                    location = telldir(dir);
                    entry = readdir(dir);
                }
            } else {
                idx++;
                location = telldir(dir);
                entry = readdir(dir);
            }
        } else {
            entry = readdir(dir);
        }
    }

    if (entry != NULL) {
        seekdir(dir, location);
        set_dir(session, dir);
        set_user_dir_idx(session, idx);
        push_action(session, PROCESSING);
        log_msg(LOG_INFO, "LIST, action pushed: %d", PROCESSING);
        return response_len;
    }

    current_line_len = sprintf(aux, ".\r\n");
    if (response_len + current_line_len < bytes) {
        response_len += current_line_len;
        strncat(response, aux, current_line_len);
    } else {
        push_action(session, PROCESSING);
        log_msg(LOG_INFO, "LIST, action pushed: %d", PROCESSING);
    }

    log_msg(LOG_INFO, "LIST, response_len: %d", response_len);
    return response_len;
}

int quit_cmd (session_ptr session) {
    DIR * user_dir = get_dir(session);
    log_msg(LOG_INFO, "quit command");

    rewinddir(user_dir);

    char path_to_mail[MAX_MAIL_PATH_LEN] = {0};
    char username[USERNAME_MAX_LEN] = {0};
    int username_len = get_username(session, username);
    strcpy(path_to_mail, get_root_dir());
    strcat(path_to_mail, "/");
    strncat(path_to_mail, username, username_len);
    strcat(path_to_mail, "/");
    int path_len = strlen(path_to_mail);

    struct dirent * entry;
    int * user_mails = get_dir_mails(session);
    int total_mails = get_dir_mails_count(session);

    int i = 0;
    while((entry = readdir(user_dir)) != NULL && i < total_mails) {
        if(entry->d_type == DT_REG) {
            if(user_mails[i]) {
                path_to_mail[path_len] = '\0';
                strcat(path_to_mail, entry->d_name);
                remove(path_to_mail);
            }
            i++;
        }
    }
    log_msg(LOG_INFO, "QUIT, mails deleted: %d", i);

    return 0;
}

int retr_cmd(session_ptr session, char * arg, int len, char * response, int bytes) {
    log_msg(LOG_INFO, "retr command");

    action_type action = pop_action(session);
    log_msg(LOG_INFO, "action: %d", action);

    if (len <= 1)
    {
        return sprintf(response, "-ERR Invalid command\r\n");
    }
    
    long msg_num = strtol(arg, NULL, 10);
    
    DIR * dir = get_dir(session);
    rewinddir(dir);

    struct dirent * entry;

    char username[USERNAME_MAX_LEN] = {0};
    char path[MAX_MAIL_PATH_LEN] = {0};
    int username_len = get_username(session, username);
    strcpy(path, get_root_dir());
    strcat(path, "/");
    strncat(path, username, username_len);
    strcat(path, "/");
    strcat(path, "cur/");

    entry = read_files(dir, msg_num);

    if (msg_num < 1 || msg_num > get_dir_mails_count(session) || entry == NULL) {
        log_msg(LOG_INFO, "There's no message %ld.", msg_num);
        return sprintf(response, "-ERR There's no message %ld.\r\n", msg_num);
    } else if (is_marked_to_delete(session, msg_num)) {
        return sprintf(response, "-ERR Message is deleted.\r\n");
    }

    struct retr_state * mail_retr_state = get_retr_state(session);
    int resp_idx = 0;

    if (action == PROCESS) {
        strcat(path, entry->d_name);
        mail_retr_state->mail_fd = open(path, O_NONBLOCK);
        if (mail_retr_state->mail_fd < 0) {
            log_msg(LOG_ERROR, "open error");
            return -1;
        }
        resp_idx = strlen("+OK\r\n");
        strncpy(response, "+OK\r\n", resp_idx);
        bytes -= resp_idx;
    }

    char mail[BUFFER_SIZE];
    int read_bytes = 0;
    byte_stuffing_state current_state = EMPTY;

    while (resp_idx < bytes && (read_bytes = read(mail_retr_state->mail_fd, mail, BUFFER_SIZE-1)) > 0)  {
        int data_idx = 0;
        mail[read_bytes] = '\0';
        for (; data_idx < read_bytes && resp_idx < bytes; data_idx++) {
            if (mail[data_idx] == '\n') {
                //response[resp_idx++] = '\r';
                current_state = LF;
            } else if (current_state == LF) {
                if (mail[data_idx] == '.') {
                    response[resp_idx++] = '.';
                }
                current_state = EMPTY;
            } else {
                if (mail[data_idx] == '\r') {
                    current_state = CR;
                }
            }
            response[resp_idx++] = mail[data_idx];
        }

        if (resp_idx == bytes) {
            lseek(mail_retr_state->mail_fd, data_idx - read_bytes, SEEK_CUR);
        }
    }
    log_msg(LOG_INFO, "RETR, read_bytes: %d", read_bytes);
    
    if (read_bytes == 0) {
        int multi_retr_len = strlen("\r\n.\r\n");
        if (resp_idx + multi_retr_len < bytes) {
            strncpy(response + resp_idx, "\r\n.\r\n", multi_retr_len);
            resp_idx += multi_retr_len;
        }
        close(mail_retr_state->mail_fd);
        log_msg(LOG_INFO, "RETR, mail closed");
    } else {
        push_action(session, PROCESSING);
        log_msg(LOG_INFO, "RETR, action pushed: %d", PROCESSING);
    }

    mail_retr_state->stuffed_byte = current_state;
    return resp_idx;
}
