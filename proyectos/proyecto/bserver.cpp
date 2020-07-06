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
    struct sockaddr_in servaddr, clientaddr;

    char * msg = (char *)"Hola cliente";
    
    s.Bind(PORT, 0);

    char buffer[MAXLINE];
    int n = s.ReceiveFrom(&clientaddr, buffer, MAXLINE);

    printf("Mensaje recibido: %s\n", buffer);

    return 0;
}