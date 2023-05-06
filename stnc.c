#define PORT 80
#include <stdio.h>
#include "src/client.h"
#include "src/server.h"
#include <string.h>
#include <stdlib.h>

int  main(int argc,char *argv[])
{
    int port;
    char *IP;

    if(!strcmp("stnc", argv[1])){
        if(!strcmp("-s", argv[2])){
            port = atoi(argv[3]);
            chat_server(port);    
        }
        else if(!strcmp("-c", argv[2])){
            IP=argv[3];
            port = atoi(argv[4]);
            chat_client(IP,port);
        }
    }

    
    return 0;
}