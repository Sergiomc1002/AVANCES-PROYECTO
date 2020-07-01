// Root request Expected output: ""

// GET / HTTP/1.1
// Host: localhost:8080
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
// Purpose: prefetch
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9,es;q=0.8


// File Request Expected output: "duck2.png"

// GET /duck2.png HTTP/1.1
// Host: localhost:8080
// Connection: keep-alive
// User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36
// Accept: image/webp,image/apng,image/*,*/*;q=0.8
// Sec-Fetch-Site: same-origin
// Sec-Fetch-Mode: no-cors
// Sec-Fetch-Dest: image
// Referer: http://localhost:8080/
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9,es;q=0.8

/*
    http_parser V1
    Objetivo: Parsear request del client para obtener el nombre del archivo que se esta solicitando
*/

// GET / HTTP/1.1
// Host: localhost:8080
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
// Purpose: prefetch
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9,es;q=0.8";

#include "http_parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <string>

/*
    header: char* containing the header
    size: size of the header
    ret: char* with the name of the file requested in the header    
*/
char* get_file_name(char* header)
{
    char* file_name = (char*)calloc(1, FILE_NAME_SIZE);

    char* command = (char*)malloc(3);
    strncpy(command, header, 3);

    bool isGetCommand = strcmp(command, "GET") == 0;

    if (!isGetCommand) 
    {
        printf("ERROR: Invalid Command: %s\n", command);
        free(file_name);
        return NULL;
    }

    // Start reading file name from pos 5 (GET /)

    int counter = 5;
    int h_size = strlen(header);
    while (header[counter] != ' ' && counter < h_size)
    {
        file_name[counter - 5] = header[counter];
        counter++;
    }

    return file_name;
}

/*
    filename: name of the file
    ret: char* header in this format:
    GET /filename HTTP/1.1
*/
char* make_request_header(char* filename) 
{
    //printf("filename: %s\n", filename);
    char* request_header = (char*)calloc(1, REQUEST_HEADER_SIZE);

    // Add Request Function
    int get_len = 5; // "GET /" -> len = 5
    strcpy(request_header, (char*)"GET /");
    // Add Filename
    int fn_size = strlen(filename);
    strcpy(request_header + get_len, filename);
    // Add Protocol
    strcpy(request_header + get_len + fn_size, (char*)" HTTP/1.1\r\nhost: www.google.com\r\n\r\n"); 

    return request_header;
}
//GET / HTTP/1.1
//HTTP/1.1 200 OK
//Content-Length: 1200
//(blank line)
//body

//req: file exists

char* get_extension_filename(char* filename) {
	char* file_type = (char*)calloc(10, sizeof(char));
	int index_filename = 0; 
	int len_filename = strlen(filename);
	while (index_filename < len_filename && *(filename+index_filename) != '.') {
		++index_filename; 
	}
	++index_filename;
	int counter = 0;  
	while(index_filename < len_filename) {
		*(file_type+counter) = *(filename+index_filename); 
		++index_filename; 
		++counter; 
	}  
	return file_type; 
}

char* make_response_header(char* filename, int content_length)
{
    char* response_header = (char*)calloc(1, REQUEST_HEADER_SIZE);
	char* end_header = (char*)"\r\n\r\n"; 
	char* end_line = (char*)"\r\n"; 
	char* len_file = (char*)"Content-Length: "; 
	char* type_file = (char*)"Content-Type: "; 
	char* file = (char*)"file/"; 				//ver linea donde dice char* exntesion, abajo. 

    char * response = (char*)"HTTP/1.1 200 OK\r\n";
    
    strcpy(response_header, response);
	strcpy(response_header+strlen(response_header), len_file); 	//no puede hacer + '0', eso daña el formato, tiene que usar strcat o strcpy, o hacerlo a pata. 
    //de momento el campo de len queda vacio, hay que agregarlo. 
    strcpy(response_header+strlen(response_header), end_line); 
    strcpy(response_header+strlen(response_header), type_file); 
    char* extension = get_extension_filename(filename);
    //en un futoro, ese file, no debe decir file, si el archivo es html, debe decir text, si es pdf debe decir application, si es imagen decir image. so on. 
    strcpy(response_header+strlen(response_header), file);
    strcpy(response_header+strlen(response_header), extension); 
    strcpy(response_header+strlen(response_header), end_header);   
    return response_header;
}


int get_file_size(char* response_header) 
{
    int size = -1;
    char* line = (char*)"Content-Length: ";
    int line_len = strlen(line);
    int line_counter = 0;
    int r_size = strlen(response_header);
    int i = 0;
    bool found_size = false;
    char* size_char = (char*)calloc(20, sizeof(char));

    while(i < r_size && !found_size)
    {
        if(response_header[i] == 'C') {
            if (strncmp(&response_header[i], line, line_len) == 0)
            {
                int counter = 0;
                while(i + line_len + counter < r_size && response_header[i + line_len + counter] != '\n' && counter < 20)
                {
                    size_char[counter] = response_header[i + line_len + counter];
                    counter++;
                }
                found_size = true;
            }
        }
        i++;
    }
    if (found_size) size = atoi(size_char);

    return size;
}


char* get_file_extension(char* response_header) {
	char* line = (char*)"Content-Type: "; 
	int line_len = strlen(line);
	int header_size = strlen(response_header);  	
	bool found_file_type = false; 
	
	char* file_type = (char*)calloc(10, sizeof(char));

	int index_data = 0;  
	while(index_data < header_size && !found_file_type) {
		if (*(response_header+index_data) == 'C') {
			if (strncmp(&response_header[index_data], line, line_len) == 0) {
				int counter = 0; 
				while(index_data + line_len+counter < header_size && response_header[index_data+line_len+counter] != '/') {
					++counter; 
				}
				++counter; 
				int counter_extesion = 0; 
				while(index_data + line_len + counter < header_size && response_header[index_data+line_len+counter] != '\r' && response_header[index_data+line_len+counter] != ';') {
					*(file_type+counter_extesion) = *(response_header+index_data+line_len+counter);
					++counter; 
					++counter_extesion; 
				}
				found_file_type = true; 
			}		
		}
		++index_data; 
	}
	return file_type; 		
}

int get_index_start_data(char* response, int read_status) {
		
	int start_data = 0; 	
	int counter_data = 0; 
	while(counter_data < read_status) {	 
		if (response[counter_data] == '\r' && response[counter_data+1] == '\n' && response[counter_data+2] == '\r' && response[counter_data+3] == '\n') {
			start_data = counter_data+4;  
			counter_data = read_status;
		}
		++counter_data; 
	}
	
return start_data; 	
}


void set_initial_values(data_arguments_t *data_arguments, char* request) {
	
	data_arguments->ip_address = (char*)calloc(200, sizeof(char)); 
	data_arguments->filename = (char*)calloc(200, sizeof(char)); 
	data_arguments->port = (char*)calloc(200, sizeof(char)); 
   
	bool found_end = false; 
	int counter = 0; 
   
   int len_request = strlen(request); 

    int server_port = 0; 
   
   
   while (!found_end && counter < len_request) {
	   if (*(request+counter) != '/' && *(request+counter) != ':') {
			*(data_arguments->ip_address+counter) = *(request+counter); 
			++counter;
		}
		else {
			
			if (*(request+counter) == ':') {
				int counter_port = 0; 
				++counter; 
				while(*(request+counter) != '/') {
					*(data_arguments->port+counter_port) = *(request+counter);
					++counter; 
					++counter_port;  
					server_port = 1; 
				}
			}
			
			found_end = true; 
			++counter; 
			int counter_filename = 0; 
			while(counter < len_request) {
				*(data_arguments->filename+counter_filename) = *(request+counter);
				++counter;
				++counter_filename; 
			}
		} 
	}
	
		
	if (server_port) {
		data_arguments->server_port = atoi(data_arguments->port); 
	}
	else {
		data_arguments->server_port = PORT_TO_CONNECT;
	}
			
}


void free_initial_values(data_arguments_t* data_arguments) {
	
	free(data_arguments->ip_address); 
	free(data_arguments->filename);
	free(data_arguments->port);  
	
}

/*
100 Continue 	Only a part of the request has been received by the server, but as long as it has not been rejected, the client should continue with the request.
101 Switching Protocols 	The server switches protocol.
200 OK 	The request is OK.
201 Created 	The request is complete, and a new resource is created .
202 Accepted 	The request is accepted for processing, but the processing is not complete.
203 Non-authoritative Information 	The information in the entity header is from a local or third-party copy, not from the original server.
204 No Content 	A status code and a header are given in the response, but there is no entity-body in the reply.
205 Reset Content 	The browser should clear the form used for this transaction for additional input.
206 Partial Content 	The server is returning partial data of the size requested. Used in response to a request specifying a Range header. The server must specify the range included in the response with the Content-Range header.
300 Multiple Choices 	A link list. The user can select a link and go to that location. Maximum five addresses  .
301 Moved Permanently 	The requested page has moved to a new url .
302 Found 	The requested page has moved temporarily to a new url .
303 See Other 	The requested page can be found under a different url .
304 Not Modified 	This is the response code to an If-Modified-Since or If-None-Match header, where the URL has not been modified since the specified date.
305 Use Proxy 	The requested URL must be accessed through the proxy mentioned in the Location header.
306 Unused 	This code was used in a previous version. It is no longer used, but the code is reserved.
307 Temporary Redirect 	The requested page has moved temporarily to a new url. 
400 Bad Request 	The server did not understand the request.
401 Unauthorized 	The requested page needs a username and a password.
402 Payment Required 	You can not use this code yet.
403 Forbidden 	Access is forbidden to the requested page.
404 Not Found 	The server can not find the requested page.
405 Method Not Allowed 	The method specified in the request is not allowed.
406 Not Acceptable 	The server can only generate a response that is not accepted by the client.
407 Proxy Authentication Required 	You must authenticate with a proxy server before this request can be served.
408 Request Timeout 	The request took longer than the server was prepared to wait.
409 Conflict 	The request could not be completed because of a conflict.
410 Gone 	The requested page is no longer available .
411 Length Required 	The "Content-Length" is not defined. The server will not accept the request without it .
412 Precondition Failed 	The pre condition given in the request evaluated to false by the server.
413 Request Entity Too Large 	The server will not accept the request, because the request entity is too large.
414 Request-url Too Long 	The server will not accept the request, because the url is too long. Occurs when you convert a "post" request to a "get" request with a long query information .
415 Unsupported Media Type 	The server will not accept the request, because the mediatype is not supported .
416 Requested Range Not Satisfiable 	The requested byte range is not available and is out of bounds.
417 Expectation Failed 	The expectation given in an Expect request-header field could not be met by this server.
500 Internal Server Error 	The request was not completed. The server met an unexpected condition.
501 Not Implemented 	The request was not completed. The server did not support the functionality required.
502 Bad Gateway 	The request was not completed. The server received an invalid response from the upstream server.
503 Service Unavailable 	The request was not completed. The server is temporarily overloading or down.
504 Gateway Timeout 	The gateway has timed out.
505 HTTP Version Not Supported 	The server does not support the "http protocol" version.
*/
char* extract_name(char * address){
	printf("%s\n",address);
	bool pare = false;
	int contador = 0;
	char* buf = new char[100];
	int index = -1;
	int length = strlen(address);
	if(length > 0){
		while(contador < length){
			if(address[contador] == '/'){
				index = contador;
			}
			contador++;
		}
		contador = 0;
		if(index != -1){
			index++;
			while(!pare && index < length){
				if(address[index] != '.'){
					buf[contador] = address[index];
				}
				else{
					buf[contador] = address[index];
					pare = true;
				}
				index++;
				contador++;
			}
		}	
		else{
			contador = 0;
			while(contador < length){
				if(address[contador] == '.'){
					buf[contador] = address[contador];
					break;
				}
				else{
					buf[contador] = address[contador];
				}
				contador++;
			}
		}
			
	}
	else{
		strcpy(buf,"index.");
	}
	
	return buf;
}

bool es_directorio(char * filename){
	bool esDirectorio = false;
	int length = strlen(filename);
	int contador = 0;
	while(contador < length){
		if(filename[contador] == '/'){
			esDirectorio = true;
			contador = length;	
		}
		contador++;
	}
	return esDirectorio;
}


int get_http_status(char* response_header, int read_status) {
	int status_http = -1;
	 
	if (strncmp(response_header+10, "200 OK", 6)) {
		status_http = 1; 
	}
	
	
	
	
	return status_http; 
}



ip_port_t* build_ip_port(char* msg) {
	
	//    "B/C/192.168.0.69/65000"
	// uffer: S/C/127.0.0.1/65000

	ip_port_t* balancer = (ip_port_t*)calloc(1, sizeof(ip_port_t));
	if ((*msg == 'B' || *msg == 'S') && (*(msg+2) == 'C' || *(msg+2) == 'D')) {
		char* ip_address = (char*)calloc(30, sizeof(char));
		char port[10];
		memset(port, 0, 10);  
		int offset = 4; 
		int index_msg = 0; 
		while (*(msg+offset+index_msg) != '/') {
			ip_address[index_msg] = msg[offset+index_msg]; 
			++index_msg; 
		}
		++index_msg;
		offset = index_msg + offset; 
		index_msg = 0;  
		int len_msg = strlen(msg);
		while ((offset + index_msg) < len_msg) {
			port[index_msg] = msg[offset+index_msg];
			++index_msg; 
		}
		balancer->ip_address = ip_address; 
		balancer->port = atoi(port);
		return balancer; 
	}
	else {
		printf("el mensaje del balanceador es incorrecto"); 
		return NULL;
	}
} 
