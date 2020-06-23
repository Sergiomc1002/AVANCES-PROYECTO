#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "http_parser.h"
#include "socket.h"
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;


#define PORT 7002

int main( int argc, char * argv[] ) {

//examples how to run the program
//./client 163.178.104.187:80/RioCeleste-VolcanTenorio.jpg
//./client 163.178.104.81	-> ecci
//./cient 163.178.104.187/index.html
//./client 163.178.104.187/ci0123/ProyectoProgramado.pdf 
//./client 163.178.104.187/ci0123/LogoECCI.png


	Socket s('d', false);
	struct sockaddr_in s_addr;
	memset(&s_addr, 0, sizeof(s_addr)); 
	
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT); 
	s_addr.sin_addr.s_addr = INADDR_ANY; 
	
	char* msg = (char*)"me cago en la pta\n";
	
	if (-1 == s.sendTo((void*)msg, strlen(msg), (void*)&s_addr)) {
		perror("something went wrong"); 
	}
	
	char* buffer = (char*)calloc(1024, sizeof(char));  
	
	int bytes_rcv = s.recvFrom((void*)buffer, 1024, (void*)&s_addr); 
	buffer[bytes_rcv] = '\0'; 
	printf("el server dice : %s \n", buffer); 





	#if 0
	Socket s( 's', false);
	char* request = argv[1]; 
	data_arguments_t *start_values = (data_arguments_t*)calloc(1, sizeof(data_arguments_t)); 
	set_initial_values(start_values, request); 
	char* file = (char*)calloc(100, sizeof(char)); 
	char* name = extract_name(start_values->filename); //recibe el nombre.extension
	int id_file;     	
	strcpy(file, name);
	request = make_request_header(start_values->filename);
	printf("REQUEST:  %s\n",request);
	int status_connect = s.Connect(start_values->ip_address, start_values->server_port );
	int write_status = s.Write(request);
	char* response = (char*)calloc(1024, sizeof(char));
	int bytes_read = 0; 
	int read_status = 0; 
	int start_data = 0; 
	int current_len = 0; 
	bool f_exit = false; 
	int status_http_protocol = 0; 


	
	while((read_status = s.Read(response, 1024)) > 0 && !f_exit) {
		if (bytes_read != 0) {
			bytes_read+= read_status;	
			write(id_file, response, read_status);
			memset(response, 0, 1024 * sizeof(char)); 	
		}
		else {	
			if ((status_http_protocol = get_http_status(response, read_status)) == 1) {	
				char* extension = get_file_extension(response); 
				strcat(file, extension); 
				id_file = creat(file, S_IRUSR | S_IWUSR);				
				start_data = get_index_start_data(response, read_status); 
				int diference = read_status - start_data; 	 
				bytes_read+= diference; 
				write(id_file, response+start_data, diference); 
				memset(response, 0, 1024 * sizeof(char));
			}
			else {
				f_exit = true; 
			}				
		}
	}


	if (status_http_protocol != 1) {			//ocurrio un error. 
		printf("something went wrong\n"); 
		switch(status_http_protocol) {
			
		}
		
	}
	close(id_file); 	
	#endif
	
	//free_initial_values(start_values); 
   
   
   
}
