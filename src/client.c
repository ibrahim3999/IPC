#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX_MESSAGE_LENGTH 1024
#define SSA  (struct sockaddr *)

void chat_client(char *ip_addr, int port) {
    int client_fd;
    struct sockaddr_in server_addr;
    char msg[MAX_MESSAGE_LENGTH];

    // create socket file descriptor
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Client -> socket failed");
        exit(EXIT_FAILURE);
    }

    // convert IP address to network order
    if (inet_pton(AF_INET, ip_addr, &server_addr.sin_addr) <= 0) {
        perror("Client -> inet_pton failed");
        exit(EXIT_FAILURE);
    }

    //set server address and port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (connect(client_fd, SSA &server_addr, sizeof(server_addr)) == -1) {
        perror("Client -> connect failed");
        exit(EXIT_FAILURE);
    }

    //set up poll to listen to both socket and keyboard input
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = client_fd;
    fds[1].events = POLLIN;

    //send and receive messages
    while (1) {
        if (poll(fds, 2, -1) == -1) {
            perror("Client -> poll failed");
            exit(EXIT_FAILURE);
        }

        //if there is keyboard input, send it to server
        if (fds[0].revents & POLLIN) {
            printf("\nClient: ");
            fgets(msg, MAX_MESSAGE_LENGTH, stdin);
            if (send(client_fd, msg, strlen(msg), 0) == -1) {
                perror("Client -> send failed");
                exit(EXIT_FAILURE);
            }
        }

        //if there is a message from server, print it to console
        if (fds[1].revents & POLLIN) {
            if (recv(client_fd, msg, MAX_MESSAGE_LENGTH, 0) <= 0) {
                perror("Client -> recv failed");
                exit(EXIT_FAILURE);
            }
            printf("\nServer: %s", msg);
        }
    }
}