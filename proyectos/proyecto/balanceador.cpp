#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define B_PORT 65000
#define PORT 7002
#define MAXLINE 1024 

int main()
{
    Socket dsocket('d', false);

    sockaddr_in s_in;

    char * msg = (char *)"B/C/192.168.0.69/65000";
    int n = dsocket.SendTo(&s_in, "", B_PORT, msg, strlen(msg));

    if (n != -1)
        printf("Mensaje enviado.\n");
}
