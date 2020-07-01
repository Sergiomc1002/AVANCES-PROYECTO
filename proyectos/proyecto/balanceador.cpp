#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iterator>
#include <list>
#include "http_parser.h"

#define B_PORT 65000
#define SERVER_PORT 7002
#define MAXLINE 1024 

typedef struct {
	Socket *server_socket;
	int client_id;
    std::list<ip_port_t*> * server_list;
} sthread_data;

typedef struct
{
    Socket * r_socket;
    Socket * s_socket;
    std::list<ip_port_t*> * server_list;
} lthread_args;

bool new_server(std::list<ip_port_t*> * server_list, ip_port_t * server_inf)
{
    std::list<ip_port_t*>::iterator it = server_list->begin();
    while(it != server_list->end())
    {
        if (strcmp((*it)->ip_address, server_inf->ip_address) == 0)
            return false;
        it++;
    }
    return true;
}

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
                if (new_server(server_list, server_inf)) 
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
            }
        }
    }


}

void * sendToServer(void * args)
{
	Socket server_socket('s',false);
    char* msg_from_client = (char*)calloc(512, sizeof(char));  

	sthread_data* thread_data = (sthread_data*)args;
    Socket * s = thread_data->server_socket;
    std::list<ip_port_t*> * server_list = thread_data->server_list;

    if(-1 == s->Read(msg_from_client,512,thread_data->client_id)) {
		perror("there was an error");
        return NULL;
	}
    printf("Message from client: %s\n", msg_from_client);

    //ip_port_t * server = chooseServer();
    ip_port_t * server = *(server_list->begin());

	int server_connect = server_socket.Connect(server->ip_address, server->port);
	server_socket.Write(msg_from_client);
	
    bool f_exit = false;
    int read_status = 0; 
    int bytes_read = 0; 
    char* response = (char*)calloc(1024, sizeof(char));
    while((read_status = server_socket.Read(response, 1024)) > 0 && !f_exit) {
		s->Write(response, thread_data->client_id,read_status);
	}
	int asd = server_socket.Shutdown();
}

int main()
{
    Socket rsocket('d', false);
    Socket ssocket('d', false);


    sockaddr_in s_in;

    char * msg = (char *)"B/C/127.0.0.1/7002";
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

    Socket server_socket('s', false);

	if(-1 == server_socket.Bind(SERVER_PORT, 0)){ // 0 = ipv4.
	perror("there was an error");
	}

	if(-1 == server_socket.Listen(10)) { 
	perror("there was an error");
	}

    while(1) {

		struct sockaddr_in socket_client;

		int client_id = server_socket.Accept(&socket_client);
		
		if (-1 == client_id) {
			perror("something went wrong");
		}
		else {
			sthread_data* sd = (sthread_data*)calloc(1,sizeof(sthread_data)); 		//hay que darle free a esto. 
			sd->server_socket = &server_socket;
			sd->client_id = client_id;  
            sd->server_list = &server_list;
			pthread_t* thread = (pthread_t*)malloc(1*sizeof(pthread_t)); 

			pthread_create(thread, NULL, sendToServer, sd); 
		}
	}


    pthread_join(ls_thread, NULL);
}
