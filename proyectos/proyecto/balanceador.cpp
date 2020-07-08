#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iterator>
#include <list>
#include "http_parser.h"

char * MY_IP;
char * msg; // mensaje a servidores
int protocol;

#define TEST 0
#define RED
//#define LOCAL


#define B_PORT 65000
#define SERVER_PORT 7002
#define MAXLINE 1024 

//CONSTANTES PARA ALGORITMOS PARA ELEGIR SERVIDORES
#define ROUNDROBIN 1
#define ROUNDROBINPESOS 2
#define DIRCLIENT 3
#define LESSCONNECTIONS 4


using namespace std;
int numServers = 0;
std::list<int> server_weigths;

typedef struct
{
	ip_port_t * server;
	char * clientIp;
} cs_pair_t;

std::list<cs_pair_t *> cs_pair_list; // lista de asignacion cliente -> servidor 

typedef struct {
	Socket *server_socket;
	int client_id;
    std::list<ip_port_t*> * server_list;
	char * client_ip;
} sthread_data;

typedef struct
{
    Socket * r_socket;
    Socket * s_socket;
    std::list<ip_port_t*> * server_list;
} lthread_args;

bool new_server(std::list<ip_port_t*> * server_list, ip_port_t * server_inf)
{
    // std::list<ip_port_t*>::iterator it = server_list->begin();
    // while(it != server_list->end())
    // {
    //     if (strcmp((*it)->ip_address, server_inf->ip_address) == 0)
    //         return false;
    //     it++;
    // }
	int peso = 1 + rand() % 3;
	printf("Peso nuevo servidor: %d\n", peso); 
	server_weigths.push_back(peso);
	numServers++;
    return true;
}

int lastServerIndex = 0;
void set_client_server(char * client_ip, std::list<ip_port_t*> * server_list)
{
	printf("Client ID: %s\n", client_ip);
	bool newClient = true;

	std::list<cs_pair_t *>::iterator it = cs_pair_list.begin();
	while (it != cs_pair_list.end())
	{
		printf("Comparando: C %s - NC %s\n", (*it)->clientIp, client_ip);
		if (strcmp((*it)->clientIp, client_ip) == 0)
			newClient = false;
		it++;
	}

	if (newClient)
	{
		cs_pair_t * cs_pair = (cs_pair_t *)calloc(1, sizeof(cs_pair_t));
		cs_pair->clientIp = client_ip;
		std::list<ip_port_t*>::iterator it = server_list->begin();
		int i = 0;
    	while(it != server_list->end() &&  i < lastServerIndex){
			it++;
		}
		cs_pair->server = *it;

		cs_pair_list.push_back(cs_pair);
		lastServerIndex = (lastServerIndex + 1) % numServers;
		printf("Nuevo Cliente:\n");
		printf("Client IP: %s Server IP: %s\n", client_ip, (*it)->ip_address);
	}
}


bool its_a_balancer(char* msg) {
	return (msg[0] == 'B'); 
}


void* listen_servers(void * args)
{
    lthread_args * ar = (lthread_args *) args;
    Socket * r_socket = ar->r_socket;
    Socket * s_socket = ar->s_socket;
    std::list<ip_port_t*> * server_list = ar->server_list;

    sockaddr_in s_in;			//cuando recibo se llena de la info. 
    int buff_size = 120;
    char buffer[buff_size];

    while(true) {
        memset(buffer, 0, buff_size);
        int n = r_socket->ReceiveFrom(&s_in, buffer, buff_size);
		printf("Esperando mensaje n: %d\n", n);
		printf("%s\n", buffer);
        //if (n != -1) {
        if (n != -1 && !its_a_balancer(buffer)) {
			printf("Protocol[MSG from Server]: %s\n", buffer);
			ip_port_t * server_inf = build_ip_port(buffer);
			if (server_inf != NULL) {
				if (is_it_a_connect_msg(buffer)) {
						printf("Si es msg conn\n");					
						if (new_server(server_list, server_inf)) 
						{
							server_list->push_back(server_inf);
							printf("New Server IP : [%s] - Port : [%d] \n", server_inf->ip_address, server_inf->port);
							
							#ifdef LOCAL
								char * msg = (char *)"B/C/127.0.0.1/65000";	//este seria el puerto para que el server me hable por stream, es indiferente.
							#endif 
							
							//int port = ntohs(s_in.sin_port);			//el server no me habla por stream, a menos que yo le pida datos. 
							int port = B_PORT + TEST; // * * 
							char * addr = inet_ntoa(s_in.sin_addr);
							printf("Sending response to : [%s] \n", addr);
							Socket socket('d', false);
							socket.SendTo(&s_in, true, port, msg, strlen(msg));
							socket.Shutdown();
						}
						else {
							//free(server_inf);
							//se le hace free a server_inf
						}
				}
				else {
					//es un mensaje de desconexion.
					std::list<ip_port_t*>::iterator it; 
					it = server_list->begin(); 
					bool found = false; 
					while (it != server_list->end()) {
						if(strcmp(server_inf->ip_address, (*it)->ip_address) == 0) {
							server_list->erase(it); 
							it = server_list->end();
							found = true;  
						}
						if (!found) { 
							++it; 
						}
					}
				}
			}
			else {
				printf("ERROR [THE MSG DOESNT FOLLOW THE PROTOCOL] \n"); 
			}
        }
        else {
			//printf("ERROR [FAILED TO RCV FROM SERVER] \n"); 
		}
        
    }


}



int roundRobinIndex = 0;
ip_port_t * roundRobin(list<ip_port_t*> * server_list){
	ip_port_t * temp = NULL;
	int index = roundRobinIndex;
	int i = 0;
	
	roundRobinIndex = (roundRobinIndex +1 ) % numServers;

	std::list<ip_port_t*>::iterator it = server_list->begin();
    while(it != server_list->end() &&  i < roundRobinIndex){
		it++;
		i++;
    }
	temp = *it;
	
	if(temp == NULL){
		printf("ERROR EN ROUNDROBIN \n");
	}
	return temp;
}
int weigthCounter = 0;
int roundRobinWeigthIndex = 0;

ip_port_t * roundRobinPesos(list<ip_port_t*> * server_list){
	ip_port_t * temp = NULL;
	
	std::list<ip_port_t*>::iterator it = server_list->begin();
	std::list<int>::iterator itWeight = server_weigths.begin();
	
	for(int i = 0; i < roundRobinWeigthIndex; ++i){
		itWeight++;
	}

	if(weigthCounter < *(itWeight)){
		weigthCounter++;
	}
	else{
		weigthCounter = 1;
		roundRobinWeigthIndex = (roundRobinWeigthIndex+1)%numServers;
	}
	
	for(int i = 0; i < roundRobinWeigthIndex; ++i){
		it++;
	}

	temp = *it;
	
	if(temp == NULL){
		printf("ERROR EN ROUNDROBINPESOS \n");
	}
	return temp;
}

ip_port_t * dirClient(char * client_ip){
	std::list<cs_pair_t *>::iterator it = cs_pair_list.begin();
	ip_port_t * temp = NULL;

	while(it != cs_pair_list.end())
	{
		if (strcmp((*it)->clientIp, client_ip) == 0)
		{
			temp = (*it)->server;
			break;
		}
		it++;
	}

	if (temp == NULL)
	{
		printf("ERROR EN DIRCLIENT\n");
	}

	return temp;
}

ip_port_t * lessConnections(){
	ip_port_t * temp = NULL;
	
	return temp;
}
ip_port_t * getServer(int algorithm , sthread_data* thread_data){
	ip_port_t * temp;
	switch(algorithm){
		case ROUNDROBINPESOS:
			temp = roundRobinPesos(thread_data->server_list);
			break;
		case DIRCLIENT:
			temp = dirClient(thread_data->client_ip);
			break;
		case LESSCONNECTIONS:
			temp = lessConnections();
			break;
		default:
			//ROUNDROBIN
			temp = roundRobin(thread_data->server_list);
			break;
	}
	return temp;
}





void * sendToServer(void * args)
{
	Socket server_socket('s',false);
    char* msg_from_client = (char*)calloc(512, sizeof(char));  

	sthread_data* thread_data = (sthread_data*)args;
    Socket * s = thread_data->server_socket;
    std::list<ip_port_t*> * server_list = thread_data->server_list;

	if (protocol == DIRCLIENT)
		set_client_server(thread_data->client_ip, server_list);

    if(-1 == s->Read(msg_from_client,512,thread_data->client_id)) {
		perror("there was an error");
        return NULL;
	}
    printf("Message from client: %s - getting ready to send it to a server ...\n", msg_from_client);

    //ip_port_t * server = chooseServer();
    //ip_port_t * server = *(server_list->begin());
    ip_port_t * server = getServer(protocol, thread_data);
	
	//imprimir el server que eligio
	printf("Server IP : [%s] - Port : [%d] \n", server->ip_address, server->port);

	if(server != NULL) {
		int server_connect = server_socket.Connect(server->ip_address, server->port);
		server_socket.Write(msg_from_client);

		int read_status = 0; 
		char* response = (char*)calloc(1024, sizeof(char));
		while((read_status = server_socket.Read(response, 1024)) > 0) {
			s->Write(response, thread_data->client_id, read_status);
		}
		
		server_socket.Shutdown();
		s->Shutdown(thread_data->client_id); 
		
	}
	else {
		//DE MOMENTO NO HAY SERVIDORES LEVANTANDOS. 
		//ENVIAR 404 NOT FOUND. 
		
		
	}
	
}

int main(int argc, char* argv[])
{
	
	
    Socket rsocket('d', false);		//para recibir cuando los servers se levantan.
    Socket ssocket('d', false);		//para avisar cuando yo me levanto. 
	rsocket.Bind(B_PORT); 
	ssocket.EnableBroadcast();

	#ifdef RED
		if (argc != 3) {
			printf("parametros invalidos \n");
			return 0;  
		}
	
		MY_IP = argv[1];
		std::string ips(MY_IP);
		std::string msgs = "B/C/" + ips + "/" + std::to_string(SERVER_PORT);
		
		msg = (char *)msgs.c_str();

		protocol = atoi(argv[2]);
	#endif

    sockaddr_in s_in;

	#ifdef LOCAL
		char * msg = (char *)"B/C/127.0.0.1/7002";
    #endif
    
    int n = ssocket.SendTo((char *)"172.16.123.31", &s_in, B_PORT + TEST, msg, strlen(msg));

    if (n != -1)
        printf("Me acabo de levantar, broadcast enviado.\n");

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

		struct sockaddr_in * socket_client = (sockaddr_in *)calloc(1, sizeof(sockaddr_in));

		int client_id = server_socket.Accept(socket_client);
		
		if (-1 == client_id) {
			perror("something went wrong");
		}
		else {
			sthread_data* sd = (sthread_data*)calloc(1,sizeof(sthread_data)); 		//hay que darle free a esto. 
			sd->server_socket = &server_socket;
			sd->client_id = client_id;  
            sd->server_list = &server_list;
			sd->client_ip = inet_ntoa(socket_client->sin_addr);
			pthread_t* thread = (pthread_t*)malloc(1*sizeof(pthread_t)); 

			pthread_create(thread, NULL, sendToServer, sd); 
		}
	}


    pthread_join(ls_thread, NULL);
}
