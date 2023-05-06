#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_LENGTH 1024
#define SSA  (struct sockaddr *) 

void chat_client(char *ip_addr, int port){
    int client_fd;
    struct sockaddr_in server_addr;
    char msg[MAX_MESSAGE_LENGTH];

    client_fd = socket(AF_INET,SOCK_STREAM,0);
    // create sokcet file descriptor
    if(client_fd == -1){
        perror("Client -> socket failed");
        exit(EXIT_FAILURE);
    }

    // convert IP address to network order
    if(inet_pton(AF_INET,ip_addr,&server_addr.sin_addr)<=0){
       perror("Client -> inet_pton failed");
        exit(EXIT_FAILURE);
    }

    //set server addres andport
     
    memset (&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(connect(client_fd,SSA&server_addr,sizeof(server_addr))==-1){
        perror("Client ->connect failed");
        exit(EXIT_FAILURE);
    }

    // send and receive msgs
    while(1){
        //send massage to server 
        printf("Clinet: ");
        fgets(msg,MAX_MESSAGE_LENGTH,stdin);
        if(send(client_fd,msg,strlen(msg),0)<0){
            perror("Client ->send falied");
            exit(EXIT_FAILURE);
        }

        //receive msg from server

        if(recv(client_fd,msg,MAX_MESSAGE_LENGTH,0) < 0 ){
            perror("Client ->recv falied");
            exit(EXIT_FAILURE);
        }
        printf("Server: %s",msg);
    }

}
void main(){
   //chat_server(1235); 
    chat_client("127.0.0.1",1235);
}
