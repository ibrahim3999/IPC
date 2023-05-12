#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define SSA  (struct sockaddr *)
#define PORT 55500
#define BUFFER_SIZE 1024
#define MMBUFFERSIZE 1024*100*16
#define LARGE_BUFFER_SIZE  104857600
#define MAX_MESSAGE_LENGTH 1024
#define REQUEST "hello world"
#define UDS_PATH "/tmp/my_unix_socket5552"
#define FILENAME "b.txt"
#define FILESIZE 100*1024*1024
#define SHARED_FILE "/my_shared_file"
#define MESSAGE_SIZE 16

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
char* generate_data(int size) {
    // Allocate memory for the string
    char* data = (char*) malloc(size + 1);

    // Seed the random number generator
    srand(time(NULL));

    // Generate random characters and store them in the string
    for (int i = 0; i < size; i++) {
        data[i] = 'a' + rand() % 26; // Generate random lowercase letter
    }

    // Add null terminator at the end of the string
    data[size] = '\0';

    return data;
}
unsigned int checksum(const char *buf, size_t len) {
    unsigned int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}
//part B
void start_server_TCP_IPv4() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char* data = generate_data(LARGE_BUFFER_SIZE);

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

    // Send data to client with checksum
    unsigned int expected_checksum = checksum(data, strlen(data));
    unsigned int received_checksum = 0;
    int total_sent = 0;

    while (total_sent < LARGE_BUFFER_SIZE) {
        int bytes_sent = send(new_socket, data, sizeof(data), 0);
        
        if (bytes_sent < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }

        total_sent += bytes_sent;
        printf("Sent %d\n" , total_sent);
    }
        //Receive Ack
        char ack[BUFFER_SIZE] = {0};
        int secceess=0;
        secceess = read(new_socket, ack, BUFFER_SIZE);
        printf("%s Received\n",ack);


    // Send checksum to client
    if (send(new_socket, &expected_checksum, sizeof(unsigned int), 0) < 0) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
    printf("Check sum sent : %u\n",expected_checksum);
    
    close(new_socket);
    close(server_fd);
}

void start_server_TCP_IPv6() {
    int server_fd, new_socket, valread;
    struct sockaddr_in6 address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char* data = generate_data(LARGE_BUFFER_SIZE);

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

    // Send data to client with checksum
    unsigned int expected_checksum = checksum(data, strlen(data));
    unsigned int received_checksum = 0;
    int total_sent = 0;

    while (total_sent < LARGE_BUFFER_SIZE) {
        int bytes_sent = send(new_socket, data, sizeof(data), 0);
        
        if (bytes_sent < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }

        total_sent += bytes_sent;
        printf("Sent %d\n" , total_sent);
    }
        //Receive Ack
        char ack[BUFFER_SIZE] = {0};
        int secceess=0;
        secceess = read(new_socket, ack, BUFFER_SIZE);
        printf("%s Received\n",ack);


    // Send checksum to client
    if (send(new_socket, &expected_checksum, sizeof(unsigned int), 0) < 0) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
    printf("Check sum sent : %u\n",expected_checksum);
    
    close(new_socket);
    close(server_fd);
}

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void start_server_UDP_IPv4(int port) {
    struct sockaddr_in server_addr, client_addr;
    int sockfd, total_sent = 0;
    socklen_t client_len = sizeof(client_addr);
    char* data = generate_data(LARGE_BUFFER_SIZE);
    unsigned int expected_checksum = checksum(data, strlen(data));
    unsigned int received_checksum = 0;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    // Receive data from client to retrieve client's address and port information
    char buffer[BUFFER_SIZE];
    if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len) == -1) {
        perror("Error receiving data");
        exit(EXIT_FAILURE);
    }
    while (total_sent < LARGE_BUFFER_SIZE) { // send 100MB of data
        int send_len = BUFFER_SIZE;
        if (total_sent + BUFFER_SIZE > LARGE_BUFFER_SIZE) {
            send_len = LARGE_BUFFER_SIZE - total_sent;
        }
        if (sendto(sockfd, data + total_sent, send_len, 0, (struct sockaddr *)&client_addr, client_len) == -1) {
            perror("Error sending data");
            exit(EXIT_FAILURE);
        }
        total_sent += send_len;
       
    }
    printf("Sent %d bytes of data\n", total_sent);
    close(sockfd);
}

void start_server_UDP_IPv6(char *ip, int port) {
    struct sockaddr_in6 server_addr, client_addr;
    int sockfd, total_sent = 0;
    socklen_t client_len = sizeof(client_addr);
    char* data = generate_data(LARGE_BUFFER_SIZE);
    unsigned int expected_checksum = checksum(data, strlen(data));
    unsigned int received_checksum = 0;

    if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));

    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    // Receive data from client to retrieve client's address and port information
    char buffer[BUFFER_SIZE];
    if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len) == -1) {
        perror("Error receiving data");
        exit(EXIT_FAILURE);
    }
    while (total_sent < LARGE_BUFFER_SIZE) { // send 100MB of data
        int send_len = BUFFER_SIZE;
        if (total_sent + BUFFER_SIZE > LARGE_BUFFER_SIZE) {
            send_len = LARGE_BUFFER_SIZE - total_sent;
        }
        if (sendto(sockfd, data + total_sent, send_len, 0, (struct sockaddr *)&client_addr, client_len) == -1) {
            perror("Error sending data");
            exit(EXIT_FAILURE);
        }
        total_sent += send_len;
       
    }
    printf("Sent %d bytes of data\n", total_sent);
    close(sockfd);
}

void start_server_UDS_dgram() {
    int sockfd, n;
    struct sockaddr_un servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char* data = generate_data(LARGE_BUFFER_SIZE);

    // Create socket file descriptor
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, UDS_PATH);

    // Bind socket to server address
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Receive request from client
    char buffer[BUFFER_SIZE];
    if ((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&cliaddr, &len)) < 0) {
        perror("error receiving request from client");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("Received request: %s\n", buffer);

    // Send 100MB of data to client
    int total_sent = 0;
    while (total_sent < LARGE_BUFFER_SIZE) {
        int send_len = BUFFER_SIZE;
        if (total_sent + BUFFER_SIZE > LARGE_BUFFER_SIZE) {
            send_len = LARGE_BUFFER_SIZE - total_sent;
        }
        if (sendto(sockfd, data + total_sent, send_len, 0, (struct sockaddr *)&cliaddr, len) == -1) {
            perror("Error sending data");
            exit(EXIT_FAILURE);
        }
        total_sent += send_len;
        printf("Sent: %d\n", total_sent);
    }

    close(sockfd);
}

void start_server_UDS_stream() {
    int server_fd, new_socket, valread;
    struct sockaddr_un address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char *hello = "Hello from server";
    char *data = generate_data(LARGE_BUFFER_SIZE);

    // Create socket file descriptor
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to UDS path
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, UDS_PATH, sizeof(address.sun_path) - 1);

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

    // Receive request from client
    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }

    printf("Received request: %s\n", buffer);

    // Send 100MB of data to client
    int total_sent = 0;
    while (total_sent < LARGE_BUFFER_SIZE) {
        int send_len = BUFFER_SIZE;
        if (total_sent + BUFFER_SIZE > LARGE_BUFFER_SIZE) {
            send_len = LARGE_BUFFER_SIZE - total_sent;
        }
        int bytes_sent = send(new_socket, data + total_sent, send_len, 0);
        if (bytes_sent < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
        total_sent += bytes_sent;
        printf("Sent: %d\n", total_sent);
    }

    close(new_socket);
    close(server_fd);
    unlink(UDS_PATH);
}


void start_server_mmap() {
    int fd;
    char* mapped;
    struct stat file_info;
    pid_t pid;
    char * data=generate_data(LARGE_BUFFER_SIZE);
    
    // Create a file for mmap
    fd = open("mmap_file", O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    // Extend the file to the buffer size
    if (ftruncate(fd, MMBUFFERSIZE) < 0) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // Map the file into memory
    mapped = mmap(NULL, MMBUFFERSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Close the file
    if (close(fd) < 0) {
        perror("close failed");
        exit(EXIT_FAILURE);
    }
    // Write data to the shared memory
    size_t total_sent=0;
    while (total_sent < LARGE_BUFFER_SIZE) {
        int send_len = LARGE_BUFFER_SIZE - total_sent;
        strncpy(mapped+total_sent, data , send_len);
        total_sent += send_len;
    }
            printf("Sucssesfully Sent %ld Bytes\n",total_sent);


    // Fork a child process to handle client connection
    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process handles client connection
        int fd;
        char buffer[MMBUFFERSIZE];
        ssize_t bytes_read;

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

        // Wait for incoming data from the server using poll
        struct pollfd poll_fd;
        poll_fd.fd = STDIN_FILENO;
        poll_fd.events = POLLIN;
        int poll_result = poll(&poll_fd, 1, -1);
        if (poll_result < 0) {
            perror("poll failed");
            exit(EXIT_FAILURE);
        }

        // Read data from the shared memory
        bytes_read = read(STDIN_FILENO, buffer, MMBUFFERSIZE);
        if (bytes_read < 0) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        // Compare the data read with the data written by the server
        if (strncmp(buffer, mapped, BUFFER_SIZE) == 0) {
            printf("\nReceived data matches!\n");
        } else {
            printf("\nReceived data does not match!\n");
        }

        // Unmap the shared memory
        if (munmap(mapped, BUFFER_SIZE) < 0) {
            perror("munmap failed");
            exit(EXIT_FAILURE);
        }

        // Exit the child process
        exit(EXIT_SUCCESS);
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define LARGE_BUFFER_SIZE (100 * 1024 * 1024)
#define BUFFER_SIZE 4096

void start_server_pipe() {
    char *data = generate_data(LARGE_BUFFER_SIZE);
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
        close(fd[0]);  // Close read end of the pipe

        // Send 100MB of data
        size_t bytes_written = 0;
        while (bytes_written < LARGE_BUFFER_SIZE) {
            size_t bytes_to_write = (LARGE_BUFFER_SIZE - bytes_written < BUFFER_SIZE) ?
                                    LARGE_BUFFER_SIZE - bytes_written : BUFFER_SIZE;
            memset(buffer, data+bytes_written, bytes_to_write);  // Fill buffer with 'A'
            ssize_t result = write(fd[1], buffer, bytes_to_write);
            if (result < 0) {
                perror("Write failed");
                exit(EXIT_FAILURE);
            }
            bytes_written += result;
        }
        printf("bytes written %ld\n",bytes_written);
        close(fd[1]);  // Close write end of the pipe
    } else {  // Child process
        close(fd[1]);  // Close write end of the pipe

        struct pollfd fd_set[1];
        fd_set[0].fd = fd[0];
        fd_set[0].events = POLLIN;

        ssize_t bytes_read;
        size_t total_bytes_read = 0;

        while (1) {
            int poll_result = poll(fd_set, 1, -1);
            if (poll_result < 0) {
                perror("Poll failed");
                exit(EXIT_FAILURE);
            }

            if (fd_set[0].revents & POLLIN) {
                bytes_read = read(fd[0], buffer, BUFFER_SIZE);
                if (bytes_read > 0) {
                    total_bytes_read += bytes_read;
                } else if (bytes_read == 0) {
                    // EOF, no more data to read
                    break;
                } else {
                    perror("Read failed");
                    exit(EXIT_FAILURE);
                }
            }
        }

        printf("Total bytes read: %ld\n", total_bytes_read);
        close(fd[0]);  // Close read end of the pipe
        exit(EXIT_SUCCESS);
    }
}



int main(){
//start_server_mmap();
start_server_pipe();
    return 0;
}
