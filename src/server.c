#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/poll.h>

#define MAX_MESSAGE_LENGTH 1024
#define SSA  (struct sockaddr *)

void chat_server(int port)
{
    int server_fd , client_fd;
    struct sockaddr_in server_addr , client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char msg[MAX_MESSAGE_LENGTH];
    struct pollfd fds[2];

    // create socket file descriptor 
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == 0){
        perror("Server -> Falied socket ");
        exit(EXIT_FAILURE);
    }

    //set socket options
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET , SO_REUSEADDR , &opt , sizeof(opt))){
        perror("Server -> Falied setsocketopt ");
        exit(EXIT_FAILURE);
    }

    memset (&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // bind socket to port 
    if(bind(server_fd,SSA &server_addr,sizeof(server_addr))<0){
        perror("Server -> Falied bind ");
        exit(EXIT_FAILURE);
    }

    //listen for incoming connections(1) 
    if(listen(server_fd,1)<0){
        perror("Server -> Falied listen ");
        exit(EXIT_FAILURE);
    }

    // accept incoming connection
    if((client_fd=accept(server_fd,SSA&client_addr,&client_addr_len))<0){
        perror("Accept -> Falied listen ");
        exit(EXIT_FAILURE);
    }

    // use poll to handle incoming messages from client and keyboard input
    fds[0].fd = client_fd;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    // send and receive messages
    while(1){
        if(poll(fds,2,-1) < 0){
            perror("Server -> Poll error");
            exit(EXIT_FAILURE);
        }

        if(fds[0].revents & POLLIN){
            //recv message from client
            if(recv(client_fd,msg ,MAX_MESSAGE_LENGTH,0)<0){
                perror("Server ->  recv Falied ");
                exit(EXIT_FAILURE);
            }
            printf("Client: %s",msg);
            strncpy(msg,"",1);
        }

        if(fds[1].revents & POLLIN){
            // read message from keyboard
            fgets(msg,MAX_MESSAGE_LENGTH,stdin);

            //send message to client
            if(send(client_fd,msg,strlen(msg),0)<0){
                perror("Server ->  send Falied ");
                exit(EXIT_FAILURE);
            }
        }
    }

}