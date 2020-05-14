#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "http_parser.h"
#include "socket.h"
#include <fcntl.h>
#include <unistd.h>
//#define SERVER_PORT 7002
#define SERVER_PORT 80



int main( int argc, char * argv[] ) {

	Socket s( 's', false);
	char* request = argv[1]; 

	data_arguments_t *start_values = (data_arguments_t*)calloc(1, sizeof(data_arguments_t)); 

	set_initial_values(start_values, request); 

	char* file = (char*)calloc(10, sizeof(char)); 

	char* name = (char*)"file."; 

	int id_file;     	

	strcpy(file, name);

	request = make_request_header(start_values->filename);

	int status_connect = s.Connect(start_values->ip_address, start_values->server_port );

	int write_status = s.Write(request);

	char* response = (char*)calloc(1024, sizeof(char));

	int bytes_read = 0; 
	int read_status = 0; 
	int start_data = 0; 
	int current_len = 0; 


	while((read_status = s.Read(response, 1024)) > 0) {
		if (bytes_read != 0) {
			bytes_read+= read_status; 	
			write(id_file, response, read_status);
			memset(response, 0, 1024 * sizeof(char)); 	
		}
		else {
				char* extension = get_file_extension(response); 
				strcat(file, extension); 
				id_file = creat(file, S_IRUSR | S_IWUSR);				
				start_data = get_index_start_data(response, read_status); 
				int diference = read_status - start_data; 	 
				bytes_read+= diference; 
				write(id_file, response+start_data, diference); 					
		}
	}


	close(id_file); 

	//free_initial_values(start_values); 
   
}

