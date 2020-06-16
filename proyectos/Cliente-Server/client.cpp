#include <stdio.h>
#include "socket.h"

#include <stdio.h>
#include <stdlib.h>

#define SERVER_PORT 7002

int main( int argc, char * argv[] ) {

   Socket s( 's', false);
	char*a = (char*)calloc(512,sizeof(char));

   int status_connect = s.Connect( "127.0.0.1", SERVER_PORT );
   int write_status = s.Write("GET / HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
   int read_status = s.Read( a, 512);
   printf( "%s\n", a);
}

