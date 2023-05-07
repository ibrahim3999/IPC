#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#include <sys/resource.h>

#define PORT 5777
#define BUFFER_SIZE 1024
#define LARGE_BUFFER_SIZE 104857600//100MB
#define MAX_MESSAGE_LENGTH 1024
#define SSA  (struct sockaddr *)

void chat_client_TCP_IPV4(char *ip_addr, int port) {
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
void client_TCP_IPv4() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[MAX_MESSAGE_LENGTH] = {0};
    char large_buffer[LARGE_BUFFER_SIZE] = {0}; // buffer to hold 100MB data

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }

    // Send message to server
    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");

   // Read response from server
    int total_bytes_read = 0;
    while (total_bytes_read < LARGE_BUFFER_SIZE) {
    valread = read(sock, buffer, BUFFER_SIZE);
    if (valread == 0) {
        // server disconnected
        break;
    }
    if (total_bytes_read + valread > LARGE_BUFFER_SIZE) {
        // limit valread to prevent buffer overflow
        valread = LARGE_BUFFER_SIZE - total_bytes_read;
    }
    memcpy(large_buffer + total_bytes_read, buffer, valread);
    total_bytes_read += valread;
}
    printf("%d bytes received from server\n", total_bytes_read);

    close(sock);
}
void client_TCP_IPv6() {
    int sock = 0, valread;
    struct sockaddr_in6 serv_addr;
    char *hello = "Hello from client";
    char buffer[MAX_MESSAGE_LENGTH] = {0};
    char large_buffer[LARGE_BUFFER_SIZE] = {0}; // buffer to hold 100MB data

    // Create socket file descriptor
    if ((sock = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET6, "::1", &serv_addr.sin6_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }

    // Send message to server
    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    // Read response from server
    int total_bytes_read = 0;
    while (total_bytes_read < LARGE_BUFFER_SIZE) {
        valread = read(sock, buffer, BUFFER_SIZE);
        if (valread == 0) {
            // server disconnected
            break;
        }
        if (total_bytes_read + valread > LARGE_BUFFER_SIZE) {
            // limit valread to prevent buffer overflow
            valread = LARGE_BUFFER_SIZE - total_bytes_read;
        }
        memcpy(large_buffer + total_bytes_read, buffer, valread);
        total_bytes_read += valread;
    }
    printf("%d bytes received from server\n", total_bytes_read);

    close(sock);
}

void main() {
    struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
    setrlimit(RLIMIT_STACK, &limit);
    client_TCP_IPv6();
}

