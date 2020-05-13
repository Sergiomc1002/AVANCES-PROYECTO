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
   
   
   while (!found_end) {
	   if (*(request+counter) != '/') {
			*(ip_address+counter) = *(request+counter); 
			++counter;
		}
		else {
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
   
   
    int id_file; 
	char* file = (char*)"index.html"; 
   
   	id_file = open(file, O_RDWR | O_CREAT | S_IRUSR | S_IWUSR);



   request = make_request_header(filename);

	printf("RE: %s", request); 

   int status_connect = s.Connect(ip_address, SERVER_PORT );

   int write_status = s.Write(request);

   char* response = (char*)calloc(512, sizeof(char));
   
   int bytes_read = 0; 
	int read_status = 0; 
	int start_data = 0; 
	int current_len = 0; 
 
	while((read_status = s.Read(response, 512)) > 0) {
		printf("entre"); 
		if (bytes_read != 0) {
			current_len = strlen(response); 
			bytes_read+= current_len; 	
			write(id_file, response, current_len);
			memset(response, 0, 512 * sizeof(char)); 	
		}
		else {		 
			/*
				int counter_data = 0; 
				int len_response = strlen(response); 
				while(counter_data < len_response) {	 
					if (response[counter_data] == '\\' && response[counter_data+1] == 'n' && response[counter_data+2] == '\\' && response[counter_data+3] == 'n') {
						start_data = counter_data+4;  
						counter_data = len_response;
					}
					++counter_data; 
				} 
			write(id_file, response+start_data, (len_response - start_data)); 		
		*/

			current_len = strlen(response); 	
			bytes_read+= current_len; 
			write(id_file, response, current_len); 	
			memset(response, 0, 512 * sizeof(char));	
		}

	}


   close(id_file); 
   
}

