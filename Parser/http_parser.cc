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
    char* file_name = (char*)malloc(FILE_NAME_SIZE);
    memset(file_name, 0, FILE_NAME_SIZE);

    char* command = (char*)malloc(3);
    strncpy(command, header, 3);

    bool isGetCommand = strcmp(command, "GET") == 0;

    if (!isGetCommand) 
    {
        printf("ERROR: Invalid Command: %s\n", command);
        free(file_name);
        return NULL;
    }

    // Start reading file name from pos 4 (GET /)

    int counter = 5;
    int h_size = strlen(header);
    while (header[counter] != ' ' && counter < h_size)
    {
        file_name[counter - 5] = header[counter];
        counter++;
    }
    
	free(command); 

    return file_name;
}
