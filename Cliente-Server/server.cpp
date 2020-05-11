#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <string.h>

#include "socket.h"

#include <pthread.h>
#include <queue>

#include <arpa/inet.h>

#define SERVER_PORT 7002



typedef struct {
Socket *server_socket;
int client_id;

}thread_data_t;


void* run(void* data) { 
	std::string answer_to_client = "mensaje recibido"; 
	char* msg_from_client = (char*)calloc(512, sizeof(char));  
	char* final_answer = (char*)calloc(512, sizeof(char));  

	thread_data_t* thread_data = (thread_data_t*)data;


	if(-1 == thread_data->server_socket->Read(msg_from_client,512,thread_data->client_id)) {
	perror("there was an error");
	}
	else {
	printf("soy el server y recibi 1 mensaje : \n %s", msg_from_client);
	}


	strcpy(final_answer, answer_to_client.c_str());		


	thread_data->server_socket->Write(final_answer, thread_data->client_id);
	
	free(msg_from_client);
	free(final_answer);  
	thread_data->server_socket->Shutdown(thread_data->client_id);
}



int main(int argc, char* argv[]) {
	
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
		thread_data_t* thread_data = (thread_data_t*)calloc(1,sizeof(thread_data_t)); 
		thread_data->server_socket = &server_socket;
		thread_data->client_id = client_id;  

		pthread_t* thread = (pthread_t*)malloc(1*sizeof(pthread_t)); 

		pthread_create(thread, NULL, run, thread_data); 
	}
}

//server_socket.Shutdown(client_id); 


return 0;
}
