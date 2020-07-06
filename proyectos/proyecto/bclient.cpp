#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "socket.h"

#define PORT 8080 
#define MAXLINE 1024 

int main()
{
    Socket s('d', false);
    struct sockaddr_in servaddr;

    char * msg = (char *)"Hola servidor";
    s.SendTo(&servaddr, PORT, msg, strlen(msg));
    
    printf("Mensaje enviado\n");
    
    return 0;
}