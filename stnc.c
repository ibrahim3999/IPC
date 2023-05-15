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

#define HOST_CONN_PORT 5656

#define CHUNK_SIZE 100 * 1024 * 1024
//void host_client(char *param ,char *type, int port) {  

void host_client(int HPORT,  char* type,  char* param) {
    printf("welcome from host clinet\n");
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char protype[2] = {0};  // Increase array size to 2 to accommodate null terminator
    // Convert protocol and type to single char
    if (strcmp(param, "tcp") == 0 && strcmp(type, "ipv4") == 0) {
        printf("welcome from host clinet --> tcp_ipv4 ...\n");
        protype[0] = '1';
    } else if (strcmp(param, "tcp") == 0 && strcmp(type, "ipv6") == 0) {
        protype[0] = '2';
    } else if (strcmp(param, "udp") == 0 && strcmp(type, "ipv4") == 0) {
        protype[0] = '3';
    } else if (strcmp(param, "udp") == 0 && strcmp(type, "ipv6") == 0) {
        protype[0] = '4';
    } else if (strcmp(param, "uds") == 0 && strcmp(type, "dgram") == 0) {
        protype[0] = '5';
    } else if (strcmp(param, "uds") == 0 && strcmp(type, "stream") == 0) {
        protype[0] = '6';
    } else if (strcmp(param, "mmap") == 0 && strcmp(type, "filename") == 0) {
        protype[0] = '7';
    } else if (strcmp(param, "pipe") == 0 && strcmp(type, "filename") == 0) {
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
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    // Connecting to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }

    // Sending message to server
    send(sock, protype, strlen(protype), 0);
    printf("Message sent to server: %s\n", protype);

    // Reading data from server
    char buffer[1024] = {0};
    valread = read(sock, buffer, 1024);
    printf("Message received from server: %s\n", buffer);
    close(sock);
}


int main(int argc, char *argv[]) {
    int port;
   
    if (argc >= 2 && !strcmp("stnc", argv[1])) {
        if (argc == 4 && !strcmp("-s", argv[2])) {

            port = atoi(argv[3]);
            chat_server_TCP_IPV4(port);

        } else if (argc == 5 && !strcmp("-c", argv[2])) {

            port = atoi(argv[4]);
            chat_client_TCP_IPv4( argv[3], port);
            
        } else if (argc == 5 && !strcmp("-s", argv[2]) && !strcmp("-p", argv[4])) {

            port = atoi(argv[3]);
            host_server(port, HOST_CONN_PORT);

        } else if (argc == 8 && !strcmp("-c", argv[2]) && !strcmp("-p", argv[5]) &&
            !strcmp("ipv4", argv[6]) && !strcmp("tcp", argv[7])) {
        
            port = atoi(argv[4]);

            host_client(HOST_CONN_PORT, argv[6],argv[7]);

            client_TCP_IPv4(argv[3], port);// segmantion fault 
          
        }  else if (argc == 8 && !strcmp("-c", argv[2]) && !strcmp("-p", argv[5]) &&
            !strcmp("ipv6", argv[6]) && !strcmp("tcp", argv[7])) { 
            
            host_client(HOST_CONN_PORT, argv[6],argv[7]); 
            port = atoi(argv[4]);
            client_TCP_IPv6(argv[3],port);

        }else if (argc == 8 && !strcmp("-c", argv[2]) && !strcmp("-p", argv[5]) &&
            !strcmp("ipv4", argv[6]) && !strcmp("udp", argv[7])) { 
            
            host_client(HOST_CONN_PORT, argv[6],argv[7]); 
            port = atoi(argv[4]);
            client_UDP_IPv4(argv[3],port);

        }else if (argc == 8 && !strcmp("-c", argv[2]) && !strcmp("-p", argv[5]) &&
            !strcmp("ipv6", argv[6]) && !strcmp("udp", argv[7])) { 
            
            host_client(HOST_CONN_PORT, argv[6],argv[7]); 
            port = atoi(argv[4]);
            client_UDP_IPv6(argv[3],port);

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
