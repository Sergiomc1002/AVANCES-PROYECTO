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
   char* ip_address = (char*)calloc(20, sizeof(char)); 
   
	bool found_end = false; 
	int counter = 0; 
   
   char* filename = (char*)calloc(20, sizeof(char)); 
   
   int len_request = strlen(request); 
   char* port = (char*)calloc(20, sizeof(char)); 
    int server_port = 0; 
   
   
   while (!found_end && counter < strlen(request)) {
	   if (*(request+counter) != '/' && *(request+counter) != ':') {
			*(ip_address+counter) = *(request+counter); 
			++counter;
		}
		else {
			
			if (*(request+counter) == ':') {
				int counter_port = 0; 
				++counter; 
				while(*(request+counter) != '/') {
					*(port+counter_port) = *(request+counter);
					++counter; 
					++counter_port;  
					server_port = 1; 
				}
			}
			
			found_end = true; 
			++counter; 
			int counter_filename = 0; 
			while(counter < len_request) {
				*(filename+counter_filename) = *(request+counter);
				++counter;
				++counter_filename; 
			}
		} 
	}
	
	
	if (server_port) {
		server_port = atoi(port); 
	}
	else {
		server_port = SERVER_PORT;
	}
   
   
    int id_file; 
    
   
   	//id_file = open(filename, O_RDWR  | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);

	//id_file = open(filename, O_RDONLY | O_CREAT);  

	
	char* file = (char*)"image.png"; 

	id_file = creat(file, S_IRUSR | S_IWUSR),

   request = make_request_header(filename);
	
   int status_connect = s.Connect(ip_address, server_port );

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
			printf("primera iteracion"); 
				int counter_data = 0; 
				while(counter_data < read_status) {	 
					if (response[counter_data] == '\r' && response[counter_data+1] == '\n' && response[counter_data+2] == '\r' && response[counter_data+3] == '\n') {
						printf("encontre el comienzo de los datos");  
						start_data = counter_data+3;  
						counter_data = read_status;
					}
					++counter_data; 
				}
			int diference = read_status - start_data; 	 
			bytes_read+= diference; 
			write(id_file, response+start_data, diference); 		
			
		}
	}
	

   close(id_file); 
   
   //free(ip_address); 
   //free(filename); 
   //free(port); 
   
}

