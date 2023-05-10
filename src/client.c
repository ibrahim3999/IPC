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
#include <errno.h>



#define PORT 1345
#define BUFFER_SIZE 1024
#define LARGE_BUFFER_SIZE 104857600//100MB
#define MAX_MESSAGE_LENGTH 1024
#define SSA  (struct sockaddr *)
#define UDS_PATH "/tmp/mysocket" //change it
#define FILENAME "100MB.bin" // put 100 mb file.



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
unsigned int checksum(const char *buf, size_t len) {
    unsigned int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}
//PARTB 
void client_TCP_IPv4() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char large_buffer[LARGE_BUFFER_SIZE] = {0}; // buffer to hold 100MB data
    unsigned int received_checksum = 0;

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

    // Receive data from server
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
        printf("Received %d\n",total_bytes_read);

    }
    // Calculate checksum of received data
    unsigned int calculated_checksum = checksum(large_buffer, LARGE_BUFFER_SIZE);

    //send Ack
    char *ack ="Ack";
    send(sock, ack, strlen(ack), 0);
    printf("Ack message sent\n");

    // Receive checksum from server
    if (recv(sock, &received_checksum, sizeof(unsigned int), 0) < 0) {
        perror("receive failed");
        exit(EXIT_FAILURE);
    }

    // Compare checksums
    if (calculated_checksum == received_checksum) {
        printf("Received %d bytes from server with checksum %u, received matching checksum\n", total_bytes_read, received_checksum);
    } else {
        printf("Received %d bytes from server with checksum %u, received non-matching checksum %u\n", total_bytes_read, received_checksum, calculated_checksum);
    }

close(sock);

}

void client_TCP_IPv6() {
    int sock = 0, valread;
    struct sockaddr_in6 serv_addr;
    char *hello = "Hello from client";
    char buffer[MAX_MESSAGE_LENGTH] = {0};
    char large_buffer[LARGE_BUFFER_SIZE] = {0}; // buffer to hold 100MB data
    unsigned int received_checksum = 0;

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
        printf("Receive %d\n", total_bytes_read);
    }
    printf("%d bytes received from server\n", total_bytes_read);

    // Calculate checksum of received data
    unsigned int calculated_checksum = checksum(large_buffer, LARGE_BUFFER_SIZE);

    //send Ack
    char *ack ="Ack";
    send(sock, ack, strlen(ack), 0);
    printf("Ack message sent\n");

    // Receive checksum from server
    if (recv(sock, &received_checksum, sizeof(unsigned int), 0) < 0) {
        perror("receive failed");
        exit(EXIT_FAILURE);
    }

    // Compare checksums
    if (calculated_checksum == received_checksum) {
        printf("Received %d bytes from server with checksum %u, received matching checksum\n", total_bytes_read, received_checksum);
    } else {
        printf("Received %d bytes from server with checksum %u, received non-matching checksum %u\n", total_bytes_read, received_checksum, calculated_checksum);
    }

    close(sock);

}
void client_UDP_IPv4(int port) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[LARGE_BUFFER_SIZE] = {0};
    char large_buffer[LARGE_BUFFER_SIZE] = {0}; // buffer to hold 100MB data
    unsigned int received_checksum = 0;
    char * conn = "connection...";

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    // Make the socket non-blocking
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
    if (sendto(sock, conn, strlen(conn), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Error sending data");
        exit(EXIT_FAILURE);
    }

    // Receive connection message from server
    int addr_len = sizeof(serv_addr);
    valread = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
    printf("%s\n", buffer);

    // Receive data from server
    int total_bytes_read = 0;
    while (total_bytes_read < LARGE_BUFFER_SIZE) {
        valread = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
        if (valread == 0 ) {
            // server disconnected
            break;
        }
        if (valread == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // No data available to read, continue loop
           // printf("dont RecvFrom:%d\n",total_bytes_read);
            total_bytes_read++;
            continue;
        }
        if (total_bytes_read + valread > LARGE_BUFFER_SIZE) {
            // limit valread to prevent buffer overflow
            valread = LARGE_BUFFER_SIZE - total_bytes_read;
        }
        memcpy(large_buffer + total_bytes_read, buffer, valread);
        total_bytes_read += valread;
        printf("Received %d\n",total_bytes_read);

        if (total_bytes_read == LARGE_BUFFER_SIZE) {
            // stop the loop if all data has been received
            break;
        }
    }

    printf("Received %d bytes of data\n", total_bytes_read);
    close(sock);
}



/*
void client_UDP_IPv6(int port) {
    struct sockaddr_in6 server_addr, client_addr;
    int sockfd, recv_len, total_recv = 0;
    socklen_t client_len;
    char buf[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        error("Error creating socket");
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        error("Error binding socket");
    }

    printf("Server started on port %d...\n", port);

    while (total_recv < LARGE_BUFFER_SIZE) { // receive 100MB of data
        memset(buf, 0, BUFFER_SIZE);
        client_len = sizeof(client_addr);

        if ((recv_len = recvfrom(sockfd, buf, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len)) == -1) {
            error("Error receiving data");
        }

        char client_addr_str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(client_addr.sin6_addr), client_addr_str, INET6_ADDRSTRLEN);

        if (sendto(sockfd, buf, recv_len, 0, (struct sockaddr *)&client_addr, client_len) == -1) {
            error("Error sending data");
        }

        total_recv += recv_len;
        printf("Received %d bytes of data\n", total_recv);

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
*/
void main() {
    struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
    setrlimit(RLIMIT_STACK, &limit);
    //client_TCP_IPv6();
    //client_TCP_IPv4();
    client_UDP_IPv4(2545);
    //client_UDP_IPv6("127.0.0.1",6666);

}