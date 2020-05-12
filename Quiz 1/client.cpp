#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "http_parser.h"
#include "socket.h"

//#define SERVER_PORT 7002
#define SERVER_PORT 80

int main( int argc, char * argv[] ) {

   Socket s( 's', false);
   
   /*
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
   
   printf("imprimiendo direccion ip : %s\n", ip_address); 
   printf("imprimiendo archivo solicitado : %s\n", filename); 
   
   */
   
   
	//char*a = (char*)calloc(512,sizeof(char));
   char* filename = argv[1];
   char* request = make_request_header(filename);
   printf("REQ: %s\n", request);

   //int status_connect = s.Connect("127.0.0.1", SERVER_PORT );
   int status_connect = s.Connect("163.178.104.187", SERVER_PORT );
   

   int write_status = s.Write(request);

   char* response = (char*)calloc(512, sizeof(char));
   int read_status = s.Read(response, 512); 

   printf("%s\n", response);

   int content_length = get_file_size(response);

   printf("Cl: %d\n", content_length);

   char* buff = response;
   int counter = 0;
   bool b_exit = false;

   std::fstream newFile;
   newFile.open("index2.html", std::fstream::in | std::fstream::out | std::fstream::app);

   while(counter < content_length && !b_exit)
   {
      // Leer
      memset(buff, 0, 512 * sizeof(char));
      int bytes_read = s.Read(buff, 512);
      if (bytes_read == -1) b_exit = true;
      else {
         counter += bytes_read;
      
         //Guardar archivo
         newFile << buff;
         //newFile << counter << '\n';
      }

   }
   
   newFile.close();
   
   
}

