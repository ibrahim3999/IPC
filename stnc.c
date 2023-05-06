#define PORT 80
#include <stdio.h>
#include "src/client.h"
#include "src/server.h"

int  main()
{
    printf("YES\n");
    chat_client("127.0.0.1",1235);
    chat_server(1235);
    
    return 0;
}