#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/poll.h>

#define SSA  (struct sockaddr *)
#define PORT 5777
#define BUFFER_SIZE 1024
#define LARGE_BUFFER_SIZE  104857600
#define MAX_MESSAGE_LENGTH 1024

void chat_server_TCP_IPV4(int port)
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
//part B
void start_server_TCP_IPv4() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char *hello = "Hello from server";

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Send 100MB (104857600 bytes) of data to client
    int total_sent = 0;
    while (total_sent < LARGE_BUFFER_SIZE) {
        int bytes_sent = send(new_socket, hello, strlen(hello), 0);
        if (bytes_sent < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
        total_sent += bytes_sent;
    }

    printf("Sent %d bytes to client\n", total_sent);
    close(new_socket);
    close(server_fd);
}
void start_server_TCP_IPv6() {
    int server_fd, new_socket, valread;
    struct sockaddr_in6 address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char *hello = "Hello from server";

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin6_family = AF_INET6;
    address.sin6_addr = in6addr_any;
    address.sin6_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Send 100MB (104857600 bytes) of data to client
    int total_sent = 0;
    while (total_sent < LARGE_BUFFER_SIZE) {
        int bytes_sent = send(new_socket, hello, strlen(hello), 0);
        if (bytes_sent < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
        total_sent += bytes_sent;
    }

    printf("Sent %d bytes to client\n", total_sent);
    close(new_socket);
    close(server_fd);
}
int main(){
    //start_server_TCP_IPv4();
    start_server_TCP_IPv6();
    return 0;
}