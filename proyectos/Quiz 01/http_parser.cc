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
    printf("filename: %s\n", filename);
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
//GET / HTTP/1.1\r\nhost: SERVIDOR LOCAL\r\n\r\n
//HTTP/1.1 200 OK
//Content-Length: 1200
//(blank line)
//body

//req: file exists
char* make_response_header(char* filename, int content_length)
{
    char* response_header = (char*)calloc(1, REQUEST_HEADER_SIZE);

    char * response = (char*)"HTTP/1.1 200 OK\nContent-Length: ";
    int r_size = strlen(response);
    strcpy(response_header, response);

    strcpy(response_header + r_size, (char*)content_length + '0');

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
