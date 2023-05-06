#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 1024
#define SSA  (struct sockaddr *)

void chat_server(int port)
{
    int server_fd , client_fd;
    struct sockaddr_in server_addr , client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char msg[MAX_MESSAGE_LENGTH];

    // create socket file descriptor 
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == -1){
        perror(" Falied socket ");
        exit(EXIT_FAILURE);
    }

    //set socket options
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET , SO_REUSEADDR , &opt , sizeof(opt))){
        perror(" Falied setsocketopt ");
        exit(EXIT_FAILURE);
    }

    // bind socket to port 
    memset (&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if(bind(server_fd,SSA&server_addr,sizeof(server_addr))<0){
        perror(" Falied bind ");
        exit(EXIT_FAILURE);
    }

    //listen for incoming connections(1) 
    if(listen(server_fd,1)<0){
        perror(" Falied listen ");
        exit(EXIT_FAILURE);
    }

    // send and receive messages
    while(1){
        //recv message from clinet
        if(recv(client_fd,msg ,MAX_MESSAGE_LENGTH,0)<0){
            perror(" recv Falied ");
            exit(EXIT_FAILURE);
        }
        printf("Clinet: %s\n",msg);

        //send msg to client 
        printf("Server: ");
        fgets(msg,MAX_MESSAGE_LENGTH,stdin);
        if(send(client_fd,msg,strlen(msg),0)<0){
            perror(" send Falied ");
            exit(EXIT_FAILURE);
        }
    }
}