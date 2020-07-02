#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include "socket.h"
#include <pthread.h>
#include <queue>
#include <arpa/inet.h>
#include "http_parser.h"
#include <list>
#define SERVER_PORT 7000
#define B_PORT 65000


typedef struct {
	std::list<ip_port_t*> * balancers; 
	Socket * r_socket; 
	Socket * s_socket; 
} listener_data_t; 

typedef struct {
	Socket *server_socket;
	int client_id;
	int thread_id; 
} thread_data_t;

void* listen_balancers(void* data) {
	listener_data_t* listener_data = (listener_data_t*)data; 
	Socket* r_socket = listener_data->r_socket; 
	Socket* s_socket = listener_data->s_socket; 
	std::list<ip_port_t*> * balancers = listener_data->balancers; 
	
	sockaddr_in s_in;
	char buffer[120];

	char * balancer_seek_msg = (char *)"S/C/127.0.0.1/7000";	//cuando me hable por stream, hableme por el 7000. 

	int n = s_socket->SendTo(&s_in, B_PORT, balancer_seek_msg, strlen(balancer_seek_msg));

	r_socket->Bind(B_PORT + 1, 0);
	n = r_socket->ReceiveFrom(&s_in, buffer, 120);

	if (n != -1) {	
		ip_port_t* balancer = build_ip_port(buffer); 
		printf("New Balancer IP : [%s] - Port : [%d] \n", balancer->ip_address, balancer->port); 
		balancers->push_back(balancer);  			
		// send	
		char * addr = inet_ntoa(s_in.sin_addr);
        printf("Sending response to : [%s] \n", addr);
		n = s_socket->SendTo(&s_in, B_PORT, balancer_seek_msg, strlen(balancer_seek_msg));
	}	
	else {
		printf("ERROR: no se pudo encontrar balanceador en la red \n");
	}
}


void* run(void* data) { 

	char* msg_from_client = (char*)calloc(512, sizeof(char));  

	thread_data_t* thread_data = (thread_data_t*)data;

	if(-1 == thread_data->server_socket->Read(msg_from_client,512,thread_data->client_id)) {
		perror("there was an error");
	}
	printf("Message from client: %s\n", msg_from_client);
	char* filename = get_file_name(msg_from_client);
	char* response_header = make_response_header(filename, 0); 		//ese 0, deberian ser la cantidad de bytes del archivo. 
	char* buffer_to_read_file = (char*)calloc(1024, sizeof(char)); 
	
	strcpy(buffer_to_read_file, response_header); 
	int len_header = strlen(buffer_to_read_file);  
	int read_status = 0;  
	int bytes_read = 0; 
	bool end_read = false;
	int file_id = -1;
	bool directorio = es_directorio(filename);
	if(directorio){
		char * temp = new char [200];
		temp[0] = '/';
		strcpy(temp+1,filename);
		file_id = open(temp, O_RDONLY);
		//delete temp;
	}
	else{
		file_id = open(filename, O_RDONLY);
	}
	if (-1 != file_id) {
		while(end_read == false) {
			if (bytes_read != 0) {
				if((read_status = read(file_id, buffer_to_read_file, 1024)) != 0) {
					
					thread_data->server_socket->Write(buffer_to_read_file, thread_data->client_id, read_status);
					memset(buffer_to_read_file, 0, 1024 * sizeof(char)); 	
					bytes_read+= read_status; 
					
				}
				else {
					end_read = true; 	
				} 		
			}
			else {
				read_status = read(file_id, buffer_to_read_file+len_header, 1024-len_header); 
				thread_data->server_socket->Write(buffer_to_read_file, thread_data->client_id, 1024);
				memset(buffer_to_read_file, 0, 1024 * sizeof(char)); 	
				bytes_read+= read_status;
				
			}	
		}		
	}
	else {
		//el archivo no existe, y hay que responderle al cliente, que lo que pide no existe. 
	}
		
	//free(filename); 
	// free(response_header); 
	// free(msg_from_client);
	// free(buffer_to_read_file); 
	thread_data->server_socket->Shutdown(thread_data->client_id);
	//free(thread_data); 
	void * asd;
	pthread_exit(asd);
}



int main(int argc, char* argv[]) {
	
	pthread_t listener_balancers; 
	listener_data_t listener_data; 
	memset(&listener_data, 0, sizeof(listener_data_t));
	Socket r_socket('d', false);
	Socket s_socket('d', false);
	std::list<ip_port_t*> balancers;	 
	listener_data.balancers = &balancers; 
	listener_data.r_socket = &r_socket;
	listener_data.s_socket = &s_socket;
	 

	pthread_create(&listener_balancers, NULL, listen_balancers, (void*)&listener_data); 
	
#if 1
	Socket server_socket('s', false);

	if(-1 == server_socket.Bind(SERVER_PORT, 0)){ // 0 = ipv4.
	perror("there was an error");
	}

	if(-1 == server_socket.Listen(10)) { 
	perror("there was an error");
	}

	int contador_threads = 0; 

	while(1) {

		struct sockaddr_in socket_client;

		int client_id = server_socket.Accept(&socket_client);
		
		if (-1 == client_id) {
			perror("something went wrong");
		}
		else {
			thread_data_t* thread_data = (thread_data_t*)calloc(1,sizeof(thread_data_t)); 		//hay que darle free a esto. 
			thread_data->server_socket = &server_socket;
			thread_data->client_id = client_id;  
			thread_data->thread_id = ++contador_threads; 
			pthread_t* thread = (pthread_t*)malloc(1*sizeof(pthread_t)); 

			pthread_create(thread, NULL, run, thread_data); 
		}
	}

	

	//server_socket.Shutdown(client_id); 

#endif 
pthread_join(listener_balancers, NULL); 

return 0;
}
