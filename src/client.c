#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/resource.h>
#include <sys/poll.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/stat.h>




#define PORT 5777
#define BUFFER_SIZE 1024
#define LARGE_BUFFER_SIZE 104857600//100MB
#define MAX_MESSAGE_LENGTH 1024
#define SSA  (struct sockaddr *)
#define UDS_PATH "/tmp/mysocket" //change it
#define FILENAME "example.txt" // put 100 mb file.

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
void client_UDP_IPv4(char *ip, int port) {
    struct sockaddr_in server_addr;
    int sockfd, i, total_sent = 0;
    char buf[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error creating socket");
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    printf("Sending 100MB data to server %s:%d...\n", ip, port);

    while (total_sent < 100000000) { // send 100MB of data
        if (sendto(sockfd, buf, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("Error sending data");
        }

        total_sent += BUFFER_SIZE;
        printf("Bytes Sent : %d\n" ,total_sent);
    }

    close(sockfd);
}


void client_UDP_IPv6(char *ip, int port) {
    struct sockaddr_in6 server_addr;
    int sockfd, i, total_sent = 0;
    char buf[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error creating socket");
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, ip, &(server_addr.sin6_addr));
    server_addr.sin6_port = htons(port);

    printf("Sending 100MB data to server %s:%d...\n", ip, port);

    while (total_sent < LARGE_BUFFER_SIZE) { // send 100MB of data
        if (sendto(sockfd, buf, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("Error sending data");
        }

        total_sent += BUFFER_SIZE;
        printf("Bytes Sent : %d\n" ,total_sent);

    }


    close(sockfd);
}

//must check the function.
void start_client_UDS_dgram() {
    int sockfd, n;
    struct sockaddr_un serv_addr;
    char buffer[BUFFER_SIZE];
    char *message = "Hello from client";

    // Create a socket file descriptor
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set the server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, UDS_PATH, sizeof(serv_addr.sun_path) - 1);

    // Send message to server
    if ((n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("sendto failed");
        exit(EXIT_FAILURE);
    }

    printf("Sent %d bytes to server\n", n);

    // Read response from server
    memset(buffer, 0, BUFFER_SIZE);
    if ((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL)) < 0) {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }

    printf("Received %d bytes from server: %s\n", n, buffer);

    // Close the socket
    close(sockfd);
}



void start_client_UDS_stream() {
    int client_fd, bytes_sent, bytes_received;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE] = {0};
    char *message = "Hello from client";

    // Create socket file descriptor
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, UDS_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    // Send message to server
    bytes_sent = send(client_fd, message, strlen(message), 0);
    if (bytes_sent < 0) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }

    // Receive response from server
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }

    printf("Received message from server: %s\n", buffer);

    close(client_fd);
}

void start_client_UDS_stream() {
    int client_fd, valread;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE] = {0};
    char *hello = "Hello from client";

    // Create socket file descriptor
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Connect to UDS path
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, UDS_PATH, sizeof(address.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    // Send message to server
    int total_sent = 0;
    while (total_sent < LARGE_BUFFER_SIZE) {
        int bytes_sent = send(client_fd, hello, strlen(hello), 0);
        if (bytes_sent < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
        total_sent += bytes_sent;
    }

    printf("Sent %d bytes to server\n", total_sent);

    // Receive response from server
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }

    printf("Received message from server: %s\n", buffer);

    close(client_fd);
}

void start_client_pipe() {
    int fd;
    char buf[1024];
    const char* message = "Hello from client";
    int message_len = strlen(message);

    // Open named pipe for writing
    if ((fd = open(FILENAME, O_WRONLY)) == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    // Send message to server
    if (write(fd, message, message_len) == -1) {
        perror("write failed");
        exit(EXIT_FAILURE);
    }

    printf("Sent message to server: %s\n", message);

    // Wait for response from server
    int bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read == -1) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    printf("Received response from server: %s\n", buf);

    close(fd);
}

void main() {
    struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
    setrlimit(RLIMIT_STACK, &limit);
    //client_TCP_IPv6();
    //client_TCP_IPv4();
    //client_UDP_IPv4("127.0.0.1",7777);
    //client_UDP_IPv6("127.0.0.1",6666);

}
