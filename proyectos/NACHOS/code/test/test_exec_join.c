#include "syscall.h"

#if 1
int main() {
	
	char * buffer = (char *)"../test/pingPong";

    int newProc = Exec(buffer);
	Join(newProc);
    char n[1];
    n[0] = newProc + '0';
    char * nP = n;
    
    Write("\ngetting out of the Join after waiting the thread # ", 52, 1); 
    Write(nP, 1, 1);
    Write("\nFINISHING....\n", 15,1); 	
 }
#endif


