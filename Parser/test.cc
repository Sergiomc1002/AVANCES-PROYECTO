#include "http_parser.h"
#include <stdio.h>
#include <stdlib.h>

char* request1 = (char *)
"GET / HTTP/1.1"
"Host: localhost:8080"
"Connection: keep-alive";
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

char* request2 = (char *) 
"GET /duck2.png HTTP/1.1"
"Host: localhost:8080";

int main()
{
    char* file_name = get_file_name(request2);
    


    if (file_name != NULL)
        printf("%s\n", file_name);

    if (file_name != NULL)
        free(file_name);

    return 0;
}