#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "server_utils.h"

#define PORT 110
#define MAX_CURRENT_CLIENTS 500


int main(int argc, char const *argv[]){

    // close stdin, stdout
    close(0);
    // close(1);


    // setup server
    int server_sock = setup_server(PORT);
    if(server_sock < 0){
        perror("setup server error");
        return 1;
    }

    unsigned int child_count = 0;


    // accept connections and delegate to child processes
    while (1){

        if(child_count > MAX_CURRENT_CLIENTS){
            printf("max clients reached\n");
            continue;
        }

        int client_sock = accept_connection(server_sock);

        if(client_sock < 0){
            perror("accept connection error");
            return 1;
        }


        // fork -> child process: handle client, parent process: close server socket
        pid_t pid = fork();
        if(pid < 0){
            perror("fork error");
        }
        else if (pid == 0){
            close(server_sock);
            handle_connection(client_sock);
            exit(0);
        }

        close(client_sock);
        child_count++;

        while(child_count){

            // end child processes
            pid = waitpid((pid_t) -1, NULL, WNOHANG);
            if(pid < 0){
                perror("waitpid error");
            }
            else if(pid == 0){
                break;
            }
            else
                child_count--;
        }
    }
    
}



