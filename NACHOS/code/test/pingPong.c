#include "syscall.h"

void SimpleThread(int);

int main( int argc, char * argv[] ) {
	
    Fork(SimpleThread);
    //SimpleThread(1);

    Write("Main  \n", 7, 1);
    //Write(argc, 4, 1);
    //Write(argv, 4, 1);
}


void SimpleThread(int num)
{
    Write("ST\n", 3, 1);
    if (num == 1) {
	for (num = 0; num < 5; num++) {
		Write("HOLAA\n", 7, 1);
		Yield();
	}
    }

    else {
	for (num = 0; num < 5; num++) {
		Write("holaa\n", 7, 1);
		Yield();
	}
    }
    Write("Fin de\n", 7, 1);
}

