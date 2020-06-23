
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLIENT_QUEUE_LEN 10  
 
Socket::Socket( char tipo, bool ipv6 ){							

	int id = 0; 
	this->ipv6 = ipv6; 


	if (ipv6) {		//ipv6
		switch(tipo) {
			case 's': 
			case 'S': 
				id = socket(AF_INET6, SOCK_STREAM, 0); 		
			break; 
			
			case 'd':
			case 'D':
				id = socket(AF_INET6, SOCK_DGRAM, 0);
			break;	
		}
	}
	else {		//ipv4. 
		switch(tipo) {
			case 's':
			case 'S':
			
				id = socket(AF_INET, SOCK_STREAM, 0); 
			break; 
			
			case 'd':
			case 'D':
				id = socket(AF_INET, SOCK_DGRAM, 0); 
			break; 
			
		}
	}

	this->id = id; 

}


Socket::~Socket(){
    //Shutdown();			
}

int Socket::Connect( char * hostip, int port ) {				
	
	int status; 	

	if (!this->ipv6) {
		struct sockaddr_in my_socket;
		my_socket.sin_addr.s_addr = inet_addr(hostip);						
		my_socket.sin_port = htons(port); 	
		my_socket.sin_family = AF_INET;		
			
		int status = connect(this->id, (struct sockaddr*)&my_socket, sizeof(my_socket)); 

	}
	else {
		struct sockaddr_in6 my_socket_ipv6;
		my_socket_ipv6.sin6_family = AF_INET6;
		inet_pton(AF_INET6, hostip, &my_socket_ipv6.sin6_addr);	
		my_socket_ipv6.sin6_port = htons(port);
		
		int status = connect(this->id, (struct sockaddr*)&my_socket_ipv6, sizeof(my_socket_ipv6)); 
		
	} 

   return status; 
}


int Socket::Connect( char *host, char *service ) { //esta la versión de connect donde no se le pasa el puerto, sino que el string http
	
	int port = 0; 
	
	if (strcmp(service, "http") == 0) {
		port = 80; 
	}
	else {
		perror("the service you wish is not possible");
		exit(0); 	
	}
	
	int status; 	

	if (!this->ipv6) {
		struct sockaddr_in my_socket;
		my_socket.sin_addr.s_addr = inet_addr(host);					
		my_socket.sin_port = htons(port); 		
		my_socket.sin_family = AF_INET;			
			
		int status = connect(this->id, (struct sockaddr*)&my_socket, sizeof(my_socket));

	}
	else {
		struct sockaddr_in6 my_socket_ipv6;
		my_socket_ipv6.sin6_family = AF_INET6;
		inet_pton(AF_INET6, host, &my_socket_ipv6.sin6_addr);	//convierte el segundo parametro en una estrucutra de red del tipo(1er parametro) y lo deja en el tercer parametro. 
		my_socket_ipv6.sin6_port = htons(port);
		
		int status = connect(this->id, (struct sockaddr*)&my_socket_ipv6, sizeof(my_socket_ipv6)); 
		
	} 

   return status; 
	
}



int Socket::Read( char *text, int len, int id_socket) {	//soy el server y quiero hacerle read de lo que me manda el cliente. 						 
	return read(id_socket, text, len);			
}


int Socket::Read(char* text, int len) {		//soy el cliente y quiero hacerle read a lo que me manda el server. 
	return read(this->id, text, len);
}




int Socket::Write( char *text, int id_socket, int length) {	//el servidor escribiendole al cliente. 
	int status = write(id_socket, text, length); 	
	if (status == -1) {
		perror("ERROR EN WRITE"); 
	}
	return status; 
}


int Socket::Write(char* text) {			//el cliente escribiendole al servidor. 
	return write(this->id, text, strlen(text)); 
}



int Socket::Listen( int queue ) {

	int status = listen(this->id, queue);
	if (-1 == status) {
		perror("error listening");
		close(this->id); 	
		return EXIT_FAILURE; 
	} 
    return status;
}



int Socket::Bind( int port, int server_client) {
	int status = 0; 
	int status_bind = 0; 
	int flag = 0; 
	
	if (0 == server_client) {			// ipv4
			
			struct sockaddr_in server_socket; 
			memset(&server_socket, 0, sizeof(server_socket)); 
			server_socket.sin_family = AF_INET;
			server_socket.sin_addr.s_addr = htonl(INADDR_ANY); 		
			server_socket.sin_port = htons(port); 
			
			flag = 1; 
			
			status = setsockopt(this->id, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));	
			if(-1 == status) {
				perror("setsockopt error"); 
				return EXIT_FAILURE; 
			}


			status_bind = bind(this->id, (struct sockaddr*)&server_socket, sizeof(server_socket)); 
			if (-1 == status_bind) {
				perror("there was an error trying to bind");
				close(this->id);
				return EXIT_FAILURE;  
			}
			
	}
	else {							// ipv6
		
		struct sockaddr_in6 server_socket_ipv6;
		server_socket_ipv6.sin6_family = AF_INET6;
		server_socket_ipv6.sin6_addr = in6addr_any;				
		server_socket_ipv6.sin6_port = htons(port);
		
		flag = 1; 
		
		
			status = setsockopt(this->id, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));	
			if(-1 == status) {
				perror("setsockopt error"); 
				return EXIT_FAILURE; 
			}		
		

			status_bind = bind(this->id, (struct sockaddr*)&server_socket_ipv6, sizeof(server_socket_ipv6)); 
			if (-1 == status_bind) {
				perror("there was an error trying to bind");
				close(this->id);		
				return EXIT_FAILURE;  
			}			
		
	}

    return status_bind;
}




int Socket::Accept(struct sockaddr_in* client_socket) {
		
	char addrs_client[INET_ADDRSTRLEN];
	
	socklen_t length_socket_client = sizeof(*client_socket); 
	
	int client_id = accept(this->id, (struct sockaddr*)client_socket, &length_socket_client); 
	if(-1 == client_id) {
		perror("accept error");
		close(this->id);
		return EXIT_FAILURE;   
	}
	else {
		inet_ntop(AF_INET, &(client_socket->sin_addr), addrs_client, sizeof(addrs_client));
		printf("New connection from: %s:%d ... \n", addrs_client, ntohs(client_socket->sin_port)); 	
	}
	
	this->client_id = client_id;
	
	return client_id; 	
}


int Socket::Accept(struct sockaddr_in6* client_socket_ipv6) {


	char addrs_client[INET6_ADDRSTRLEN];
	
	socklen_t length_socket_client = sizeof(*client_socket_ipv6); 
	
	int client_id = accept(this->id, (struct sockaddr*)client_socket_ipv6, &length_socket_client); 
	if(-1 == client_id) {
		perror("accept error");
		close(this->id);
		return EXIT_FAILURE;   
	}
	else {
		inet_ntop(AF_INET6, &(client_socket_ipv6->sin6_addr), addrs_client, sizeof(addrs_client));
		printf("New connection from: %s:%d ... \n", addrs_client, ntohs(client_socket_ipv6->sin6_port)); 	
	}
	
	this->client_id = client_id;
	
	return client_id; 		
	
}



void Socket::SetIDSocket(int id){
    this->id = id;
}

int Socket::Shutdown() {
	printf("CONECTION CLOSED");
	return close(this->id);
	
}


int Socket::Shutdown(int client_id) {
	printf("CONECTION CLOSED");
	return close(this->client_id);
}


int Socket::recvFrom(void* buffer, int len_buffer, void* sockaddr) {
	socklen_t size = sizeof(struct sockaddr); 
	return recvfrom (this->id, buffer, len_buffer, 0, (struct sockaddr *) sockaddr, &size);	
}

 
int Socket::sendTo(void* msg, int len_msg, void* sockaddr) {
	socklen_t size = sizeof(struct sockaddr); 
	return sendto (this->id, msg, len_msg, 0, (struct sockaddr *) & sockaddr, size);
}

