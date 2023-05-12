

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
#include <sys/mman.h>


#define PORT 1345
#define BUFFER_SIZE 1024
#define LARGE_BUFFER_SIZE 104857600//100MB
#define MAX_MESSAGE_LENGTH 1024
#define SSA  (struct sockaddr *)
#define UDS_PATH "/tmp/my_unix_socket5552"
#define REQUEST "hello world"
#define FILENAME "b.txt"
#define FILESIZE 100*1024*1024
#define SHARED_FILE "/my_shared_file"
#define MESSAGE_SIZE 16



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
    struct pollfd fdset[1];
    fdset[0].fd = sock;
    fdset[0].events = POLLIN;
    int ret = poll(fdset, 1, 3000);
    if (ret == -1) {
        perror("Error polling socket");
        exit(EXIT_FAILURE);
    } else if (ret == 0) {
        printf("Timeout waiting for connection message\n");
        exit(EXIT_FAILURE);
    } else {
        valread = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
        printf("%s\n", buffer);
    }

    // Receive data from server
    int total_bytes_read = 0;
    while (total_bytes_read < LARGE_BUFFER_SIZE) {
        struct pollfd fdset[1];
        fdset[0].fd = sock;
        fdset[0].events = POLLIN;
        int ret = poll(fdset, 1, 3000);
        if (ret == -1) {
            perror("Error polling socket");
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            printf("total read :%d\n",total_bytes_read);
            printf("Timeout waiting for data\n");
            exit(EXIT_FAILURE);
        } else {
            valread = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
            if (valread == 0 ) {
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

            if (total_bytes_read == LARGE_BUFFER_SIZE) {
                // stop the loop if all data has been received
                break;
            }
        }
    }
    printf("Received %d bytes of data\n", total_bytes_read);
    close(sock);
}

void client_UDP_IPv6(int port) {
    int sock = 0, valread;
    struct sockaddr_in6 serv_addr;
    char buffer[LARGE_BUFFER_SIZE] = {0};
    char large_buffer[LARGE_BUFFER_SIZE] = {0}; // buffer to hold 100MB data
    unsigned int received_checksum = 0;
    char * conn = "connection...";

    // Create socket file descriptor
    if ((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    // Make the socket non-blocking
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET6, "::1", &serv_addr.sin6_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
    if (sendto(sock, conn, strlen(conn), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Error sending data");
        exit(EXIT_FAILURE);
    }

    // Receive connection message from server
    int addr_len = sizeof(serv_addr);
    struct pollfd fdset[1];
    fdset[0].fd = sock;
    fdset[0].events = POLLIN;
    int ret = poll(fdset, 1, 3000);
    if (ret == -1) {
        perror("Error polling socket");
        exit(EXIT_FAILURE);
    } else if (ret == 0) {
        printf("Timeout waiting for connection message\n");
        exit(EXIT_FAILURE);
    } else {
        valread = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
        printf("%s\n", buffer);
    }

    // Receive data from server
    int total_bytes_read = 0;
    while (total_bytes_read < LARGE_BUFFER_SIZE) {
        struct pollfd fdset[1];
        fdset[0].fd = sock;
        fdset[0].events = POLLIN;
        int ret = poll(fdset, 1, 3000);
        if (ret == -1) {
            perror("Error polling socket");
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            printf("total read :%d\n",total_bytes_read);
            printf("Timeout waiting for data\n");
            exit(EXIT_FAILURE);
        } else {
            valread = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
            if (valread == 0 ) {
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

            if (total_bytes_read == LARGE_BUFFER_SIZE) {
                // stop the loop if all data has been received
                break;
            }
        }
    }
    printf("Received %d bytes of data\n", total_bytes_read);
    close(sock);
}
//must check the function.
void client_UDS_dgram() {
    int sock = 0, n;
    struct sockaddr_un serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char large_buffer[LARGE_BUFFER_SIZE] = {0}; // buffer to hold 100MB data
    unsigned int received_checksum = 0;

    // Create socket file descriptor
    if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, UDS_PATH, sizeof(serv_addr.sun_path)-1);

    // Bind socket to a random path on the file system
    struct sockaddr_un local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sun_family = AF_UNIX;
    char temp_path[] = "/tmp/udsXXXXXX";
    int fd = mkstemp(temp_path);
    close(fd);
    unlink(temp_path);
    strncpy(local_addr.sun_path, temp_path, sizeof(local_addr.sun_path)-1);
    if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) == -1) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Connect the socket to the server's path
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect error");
        exit(EXIT_FAILURE);
    }

    // Send request to server
    int req_len = strlen(REQUEST);
    if (send(sock, REQUEST, req_len, 0) != req_len) {
        perror("Request send failed");
        exit(EXIT_FAILURE);
    }

    // Receive data from server
    int total_bytes_read = 0;
    while (total_bytes_read < LARGE_BUFFER_SIZE) {
        n = recv(sock, buffer, BUFFER_SIZE, 0);
        if (n == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            // server disconnected
            break;
        }
        if (total_bytes_read + n > LARGE_BUFFER_SIZE) {
            // limit n to prevent buffer overflow
            n = LARGE_BUFFER_SIZE - total_bytes_read;
        }
        memcpy(large_buffer + total_bytes_read, buffer, n);
        total_bytes_read += n;
        printf("Received %d\n",total_bytes_read);

        if (total_bytes_read == LARGE_BUFFER_SIZE) {
            // stop the loop if all data has been received
            break;
        }
    }
    printf("Received %d bytes of data\n", total_bytes_read);

    // Close the socket and delete the temporary file
    close(sock);
    unlink(local_addr.sun_path);
}

void client_UDS_stream() {
    int sock = 0, n;
    struct sockaddr_un serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char large_buffer[LARGE_BUFFER_SIZE] = {0}; // buffer to hold 100MB data
    unsigned int received_checksum = 0;

    // Create socket file descriptor
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, UDS_PATH, sizeof(serv_addr.sun_path)-1);

    // Connect the socket to the server's path
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect error");
        exit(EXIT_FAILURE);
    }

    // Send request to server
    int req_len = strlen(REQUEST);
    if (send(sock, REQUEST, req_len, 0) != req_len) {
        perror("Request send failed");
        exit(EXIT_FAILURE);
    }

    // Receive data from server
    int total_bytes_read = 0;
    while (total_bytes_read < LARGE_BUFFER_SIZE) {
        n = recv(sock, buffer, BUFFER_SIZE, 0);
        if (n == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            // server disconnected
            break;
        }
        if (total_bytes_read + n > LARGE_BUFFER_SIZE) {
            // limit n to prevent buffer overflow
            n = LARGE_BUFFER_SIZE - total_bytes_read;
        }
        memcpy(large_buffer + total_bytes_read, buffer, n);
        total_bytes_read += n;
        printf("Received %d\n",total_bytes_read);

        if (total_bytes_read == LARGE_BUFFER_SIZE) {
            // stop the loop if all data has been received
            break;
        }
    }
    printf("Received %d bytes of data\n", total_bytes_read);

    // Close the socket
    close(sock);
}

#define MMBUFFERSIZE 100*1024*1024 // 100MB

void client_mmap() {
    int fd;
    char* mapped;
    struct stat file_info;

    // Open the mmap file
    fd = open("mmap_file", O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    // Map the file into memory
    mapped = mmap(NULL, MMBUFFERSIZE, PROT_READ, MAP_SHARED , fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Close the file
    if (close(fd) < 0) {
        perror("close failed");
        exit(EXIT_FAILURE);
    }

    // Read data from the shared memory in chunks
    size_t total_bytes_read = 0;
    while (total_bytes_read < MMBUFFERSIZE) {
        size_t bytes_to_read = MMBUFFERSIZE - total_bytes_read;
        if (bytes_to_read > BUFFER_SIZE) {
            bytes_to_read = BUFFER_SIZE;
        }
        total_bytes_read += bytes_to_read;
    }
        printf("\nReading %lu bytes from the shared memory...\n", total_bytes_read);

    // Unmap the shared memory
    if (munmap(mapped, MMBUFFERSIZE) < 0) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }
}



void client_pipe() {
    int fd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    if (pipe(fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {  // Parent process
        close(fd[1]);  // Close write end of the pipe

        ssize_t bytes_read;
        size_t total_bytes_read = 0;
        while ((bytes_read = read(fd[0], buffer, BUFFER_SIZE)) > 0) {
            total_bytes_read += bytes_read;
            printf("Read %ld bytes from server: %s\n", bytes_read, buffer);
        }

        if (bytes_read < 0) {
            perror("Read failed");
            exit(EXIT_FAILURE);
        }

        printf("Total bytes read: %ld\n", total_bytes_read);
        close(fd[0]);  // Close read end of the pipe
        exit(EXIT_SUCCESS);
    } else {  // Child process
        close(fd[0]);  // Close read end of the pipe
        close(fd[1]);  // Close write end of the pipe
        exit(EXIT_SUCCESS);
    }
}

void main() {
    struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
    setrlimit(RLIMIT_STACK, &limit);
  //  client_mmap();
  client_pipe();

} 



