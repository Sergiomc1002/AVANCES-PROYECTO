#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <list>
#include "http_parser.h"

#define B_PORT 65000
#define PORT 7002
#define MAXLINE 1024 

typedef struct
{
    Socket * r_socket;
    Socket * s_socket;
    std::list<ip_port_t*> * server_list;
} lthread_args;

void* listen_servers(void * args)
{
    lthread_args * ar = (lthread_args *) args;
    Socket * r_socket = ar->r_socket;
    Socket * s_socket = ar->s_socket;
    std::list<ip_port_t*> * server_list = ar->server_list;

    int r = r_socket->Bind(B_PORT, 0);

    sockaddr_in s_in;
    int buff_size = 120;
    char buffer[buff_size];

    while(true) {
        memset(buffer, 0, buff_size);
        int n = r_socket->ReceiveFrom(&s_in, buffer, buff_size);

        if (n != -1) 
        {
            printf("Buffer: %s\n", buffer);
            ip_port_t * server_inf = build_ip_port(buffer);
            if (server_inf != NULL)
            {
                server_list->push_back(server_inf);
                printf("Nuevo servidor IP: %s\n", server_inf->ip_address);
                char * msg = (char *)"B/C/127.0.0.1/65000";
                //int port = ntohs(s_in.sin_port);
                int port = B_PORT + 1; // * * 
                char * addr = inet_ntoa(s_in.sin_addr);
                printf("IP Servidor: %s\n", addr);

                s_socket->SendTo(&s_in, addr, port, msg, strlen(msg));
            }
            else 
            {
                
            }
        }
    }


}

int main()
{
    Socket rsocket('d', false);
    Socket ssocket('d', false);


    sockaddr_in s_in;

    char * msg = (char *)"B/C/127.0.0.1/65000";
    int n = ssocket.SendTo(&s_in, "", B_PORT + 1, msg, strlen(msg));

    if (n != -1)
        printf("Mensaje enviado.\n");

    pthread_t ls_thread;
    std::list<ip_port_t*> server_list;
    lthread_args args;
    args.r_socket = &rsocket;
    args.s_socket = &ssocket;
    args.server_list = &server_list;

    pthread_create(&ls_thread, NULL, listen_servers, (void *)&args);

    pthread_join(ls_thread, NULL);
}
