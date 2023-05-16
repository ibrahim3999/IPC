#include <stdio.h>
#include "src/client.h"
#include "src/server.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#define HOST_CONN_PORT 5723
#define PORT_time 6787

#define CHUNK_SIZE 100 * 1024 * 1024
//void host_client(char *param ,char *type, int port) {  

void host_client(int HPORT, char* type, char* param) {
    if (type == NULL || param == NULL) {
        printf("Type or parameter is null.\n");
        exit(EXIT_FAILURE);
    }
    printf("welcome from host clinet\n");
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char protype[2] = {0};  // Increase array size to 2 to accommodate null terminator
    // Convert protocol and type to single char
    if (strcmp(param, "tcp") == 0 && strcmp(type, "ipv4") == 0) {
        protype[0] = '1';
    } else if (strcmp(param, "tcp") == 0 && strcmp(type, "ipv6") == 0) {
        protype[0] = '2';
    } else if (strcmp(param, "udp") == 0 && strcmp(type, "ipv4") == 0) {
        protype[0] = '3';
    } else if (strcmp(param, "udp") == 0 && strcmp(type, "ipv6") == 0) {
        protype[0] = '4';
    } else if (strcmp(param, "dgram") == 0 && strcmp(type, "uds") == 0) {
        protype[0] = '5';
    } else if (strcmp(param, "stream") == 0 && strcmp(type, "uds") == 0) {
        protype[0] = '6';
    } else if (strcmp(param, "filename") == 0 && strcmp(type, "mmap") == 0) {
        protype[0] = '7';
    } else if (strcmp(param, "filename") == 0 && strcmp(type, "pipe") == 0) {
        protype[0] = '8';
    } else {
        printf("Invalid protocol or type\n");
        printf("param=%s\n",param);
        printf("type=%s\n",type);
        exit(EXIT_FAILURE);
    }

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(HPORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nin host Invalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    // Connecting to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }

    // Sending message to server
    send(sock, protype, strlen(protype), 0);

    // Reading data from server
    char buffer[1024] = {0};
    valread = read(sock, buffer, 1024);
    close(sock);
}

void server_tcp_time(double num) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        perror("socket failed");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT_time);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
        perror("bind failed in time");

    if (listen(server_fd, 3) < 0)
        perror("listen failed");

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        perror("accept failed");

    // Convert the double number to a character array and send it to the client
    snprintf(buffer, sizeof(buffer), "%f", num);
    send(new_socket, buffer, strlen(buffer), 0);
    printf("Message sent: %s\n", buffer);

    close(new_socket);
    close(server_fd);
}

double client_tcp_time() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("socket failed");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_time);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
        perror("invalid address");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        perror("connection failed in time");

    // Receive the message from the server into the buffer
    valread = read(sock, buffer, 1024);

    // Convert the character array to a double and print it
    double num = strtod(buffer, NULL);
    close(sock);
    return num;
}


int main(int argc, char *argv[]) {
    int port = 0;
    double elapsed_time;

    if (argc >= 2 && argv[1] != NULL && !strcmp("stnc", argv[1])) {
        if (argc == 4 && argv[2] != NULL && !strcmp("-s", argv[2])) {
            port = atoi(argv[3]);
            chat_server_TCP_IPV4(port);
        }
        else if (argc == 5 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL) {
            port = atoi(argv[4]);
            chat_client_TCP_IPv4(argv[3], port);
        }
        else if (argc == 5 && argv[2] != NULL && !strcmp("-s", argv[2]) && argv[4] != NULL && !strcmp("-p", argv[4])) {
            port = atoi(argv[3]);
            host_server(port, HOST_CONN_PORT);
        }
        else if (argc == 6 && argv[2] != NULL && !strcmp("-s", argv[2]) && argv[4] != NULL && !strcmp("-p", argv[4]) && argv[5] != NULL && !strcmp("-q", argv[5])) {
        port = atoi(argv[3]);

        int par; 
        par=host_server(port, HOST_CONN_PORT);
        sleep(10);
        double tmp_time=0.0;
        tmp_time = client_tcp_time();
        switch (par)
        {
        case 1:
            printf("ipv4_tcp,%.2f\n",tmp_time);
            break;
        case 2:
            printf("ipv6_tcp,%.2f\n",tmp_time);
            break;
        case 3:
            printf("ipv4_udp,%.2f\n",tmp_time);
            break;
        case 4:
            printf("ipv6_udp,%.2f\n",tmp_time);
            break;
        case 5:
            printf("uds_dgram,%.2f\n",tmp_time);
            break;
        case 6:
            printf("uds_stream,%.2f\n",tmp_time);
            break;
        case 7:
            printf("mmap,%.2f\n",tmp_time);
            break;
        case 8:
            printf("pipe,%.2f\n",tmp_time);
            break;
                                                                
        default:
            break;
        }

        }          
        else if (argc == 8 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL &&
            argv[5] != NULL && !strcmp("-p", argv[5]) && !strcmp("ipv4", argv[6]) && !strcmp("tcp", argv[7])) {
            port = atoi(argv[4]);

            struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
            setrlimit(RLIMIT_STACK, &limit);  

            clock_t start_time = clock();
            host_client(HOST_CONN_PORT, argv[6], argv[7]);
            sleep(3);
            client_TCP_IPv4(argv[3], port);
            clock_t end_time = clock();
            elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
            server_tcp_time(elapsed_time);
            printf("Time spent in host_client() and client_TCP_IPv4(): %.2f milliseconds\n", elapsed_time);

        }

        else if (argc == 8 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL &&
            argv[5] != NULL && !strcmp("-p", argv[5]) && !strcmp("ipv6", argv[6]) && !strcmp("tcp", argv[7])) {
                port = atoi(argv[4]);

                struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
                setrlimit(RLIMIT_STACK, &limit);           

                clock_t start_time = clock();
                host_client(HOST_CONN_PORT, argv[6], argv[7]);
                sleep(3);
                client_TCP_IPv6(argv[3], port);
                clock_t end_time = clock();
                elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
                server_tcp_time(elapsed_time);
                //sleep(3);
                printf("Time spent: %.2f milliseconds\n", elapsed_time);

        } 
        else if (argc == 8 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL &&
                   argv[5] != NULL && !strcmp("-p", argv[5]) && !strcmp("ipv4", argv[6]) && !strcmp("udp", argv[7])) {
                     port = atoi(argv[4]);

            struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
            setrlimit(RLIMIT_STACK, &limit);    

            clock_t start_time = clock();
            host_client(HOST_CONN_PORT, argv[6], argv[7]);
            sleep(5);
            printf("Before\n");
            client_UDP_IPv4(argv[3], port);
            sleep(3);
            printf("After\n");
            clock_t end_time = clock();
            elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
            server_tcp_time(elapsed_time);
            printf("Time spent: %.2f milliseconds\n", elapsed_time);
        }
         else if (argc == 8 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL &&
                   argv[5] != NULL && !strcmp("-p", argv[5]) && !strcmp("ipv6", argv[6]) && !strcmp("udp", argv[7])) {
            port = atoi(argv[4]);
            struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
            setrlimit(RLIMIT_STACK, &limit);        
            clock_t start_time = clock();
            host_client(HOST_CONN_PORT, argv[6], argv[7]);
            sleep(3);
            client_UDP_IPv6(argv[3], port);
            clock_t end_time = clock();
            elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
            server_tcp_time(elapsed_time);
            printf("Time spent : %.2f milliseconds\n", elapsed_time);
        }
         else if (argc == 8 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL &&
                   argv[5] != NULL && !strcmp("-p", argv[5]) && !strcmp("uds", argv[6]) && !strcmp("dgram", argv[7])) {
            struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
            setrlimit(RLIMIT_STACK, &limit);        
            clock_t start_time = clock();
            host_client(HOST_CONN_PORT, argv[6], argv[7]);
            sleep(3);
            client_UDS_dgram();
            clock_t end_time = clock();
            elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
            server_tcp_time(elapsed_time);
            printf("Time spent: %.2f milliseconds\n", elapsed_time);
        }    
         else if (argc == 8 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL &&
                   argv[5] != NULL && !strcmp("-p", argv[5]) && !strcmp("uds", argv[6]) && !strcmp("stream", argv[7])) {
            struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
            setrlimit(RLIMIT_STACK, &limit);  
            clock_t start_time = clock();
            host_client(HOST_CONN_PORT, argv[6], argv[7]);
            sleep(3);
            client_UDS_stream();
            clock_t end_time = clock();
            elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
            server_tcp_time(elapsed_time);
            printf("Time spent: %.2f milliseconds\n", elapsed_time);
        }  
         else if (argc == 8 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL &&
                   argv[5] != NULL && !strcmp("-p", argv[5]) && !strcmp("mmap", argv[6]) && !strcmp("filename", argv[7])) {
            struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
            setrlimit(RLIMIT_STACK, &limit);        
            clock_t start_time = clock();
            host_client(HOST_CONN_PORT, argv[6], argv[7]);
            sleep(3);
            client_mmap();
            clock_t end_time = clock();
            elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
            server_tcp_time(elapsed_time);
            printf("Time spent in host_client() and client_TCP_IPv4(): %.2f milliseconds\n", elapsed_time);
        } 
         else if (argc == 8 && argv[2] != NULL && !strcmp("-c", argv[2]) && argv[3] != NULL &&
                   argv[5] != NULL && !strcmp("-p", argv[5]) && !strcmp("pipe", argv[6]) && !strcmp("filename", argv[7])) {
            struct rlimit limit = { .rlim_cur = 1024 * 1024 * 1024, .rlim_max = 1024 * 1024 * 1024 };
            setrlimit(RLIMIT_STACK, &limit);        
            clock_t start_time = clock();
            host_client(HOST_CONN_PORT, argv[6], argv[7]);
            sleep(3);
            client_pipe();
            clock_t end_time = clock();
            elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
            server_tcp_time(elapsed_time);
            printf("Time spent in host_client() and client_TCP_IPv4(): %.2f milliseconds\n", elapsed_time);
        }                              
        else {
            printf("Invalid command line arguments\n");
            return 1;
        }
        
    } else {
        printf("Invalid command line arguments\n");
        return 1;
    }

    return 0;
}
