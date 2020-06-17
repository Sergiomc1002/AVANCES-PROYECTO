#include "syscall.h"

int main()
{
    //char buffer[60] = "pingPong";
    char * buffer = (char *)"../test/pingPong";

    int newProc = Exec(buffer);
    char n[3];
    n[0] = newProc + '0';
    n[1] = 'Z';
    n[2] = '\n';
    char * nP = n;
    Write(nP, 3, 1);
 	Join(newProc);

}

// int
// main()
// {
//     SpaceId newProc;
//     OpenFileId input = ConsoleInput;
//     OpenFileId output = ConsoleOutput;
//     char prompt[2], ch, buffer[60];
//     int i;


//     prompt[0] = '-';
//     prompt[1] = '-';

//     while( 1 )
//     {
// 	Write(prompt, 2, output);

// 	i = 0;
	
// 	do {
	
// 	    Read(&buffer[i], 1, input); 

// 	} while( buffer[i++] != '\n' );

// 	buffer[--i] = '\0';

// 	if( i > 0 ) {
// 		newProc = Exec(buffer);
//         prinft("New Proc: %d\n", newProc);
// 		Join(newProc);
// 	}
//     }
// }

