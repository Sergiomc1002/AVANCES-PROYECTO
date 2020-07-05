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
#define SERVER_PORT 7002
#define B_PORT 65000

char * MY_IP;

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

	std::string ips(MY_IP);
	std::string balancer_seek_msg = "S/C/" + ips + "/" + std::to_string(SERVER_PORT);
	const char * msgp = balancer_seek_msg.c_str();
	//har * balancer_seek_msg = (char *)"S/C/127.0.0.1/7000";	//cuando me hable por stream, hableme por el 7000. 

	int n = s_socket->SendTo(&s_in, B_PORT, (char *)msgp, strlen(msgp));
	printf("Me acabo de levantar, broadcast enviado.\n");

	//r_socket->Bind(B_PORT + 1, 0);
	
	while(true) {
		memset(buffer, 0, 120);
		memset(&s_in, 0, sizeof(sockaddr_in)); 
		n = r_socket->ReceiveFrom(&s_in, buffer, 120);

		if (n != -1) {	
			ip_port_t* balancer = build_ip_port(buffer); 
			printf("New Balancer IP : [%s] - Port : [%d] \n", balancer->ip_address, balancer->port); 
			balancers->push_back(balancer);  			
			// send	
			char * addr = inet_ntoa(s_in.sin_addr);
			printf("Sending response to : [%s] to Port : [%d] \n", addr, B_PORT);
			n = s_socket->SendTo(&s_in, B_PORT, (char *)msgp, strlen(msgp));
			printf("response sent \n"); 
		}	
		else {
			printf("ERROR: no se pudo encontrar balanceador en la red \n");
		}		
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
	char* response_header; 
	
	int option = -1; 
	
	if (!strcmp(filename, "400 Bad Request")) {
		response_header = make_response_header(filename, 0, 400);
		option = 400;  
	}
	else {
		if (!strcmp(filename, "505 HTTP Version Not Supported")) {
			response_header = make_response_header(filename, 0, 505); 
			option = 505; 
		}
		else {
			if (!strcmp(filename, "501 Not Implemented")) {
				response_header = make_response_header(filename, 0, 501); 
				option = 501; 
			}
			else {
				response_header = make_response_header(filename, 0, 200); 		//ese 0, deberian ser la cantidad de bytes del archivo. 
				option = 200; 
			}
		}
	}
	
	if (option == 200) {
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
				memset(buffer_to_read_file, 0, 1024 * sizeof(char)); 
				free(response_header);
				response_header = make_response_header("", 0, 404);
				thread_data->server_socket->Write(response_header, thread_data->client_id, strlen(response_header)); 
				//aqui sería hacer el 404 not found. 
			}
	}
	else {
			thread_data->server_socket->Write(response_header, thread_data->client_id, strlen(response_header)); 
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
	r_socket.Bind(B_PORT, 0); 
	Socket s_socket('d', false);
	std::list<ip_port_t*> balancers;	 
	listener_data.balancers = &balancers; 
	listener_data.r_socket = &r_socket;
	listener_data.s_socket = &s_socket;
	 

	MY_IP = argv[1];	

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
