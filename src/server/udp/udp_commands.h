#ifndef _UDP_COMMANDS_H_
#define _UDP_COMMANDS_H_

#define OK 20
#define CLIENT_ERROR 40
#define CLIENT_ERROR_UNAUTHORIZED 41
#define USER_DOES_NOT_EXIST 42
#define SERVER_ERROR 50


struct udp_rqst {
    int ip_version;
    char username[32];
    char password[32];
    int id;
    char *command;
    char *arg1;
    char *arg2;
};

struct udp_command {
    char *command;
};

void udp_get_bytes(char *buffer);

void udp_get_current(char *buffer);

void udp_get_history(char *buffer);

void udp_change_password(char *buffer);

void udp_delete_user(char *buffer);

#endif