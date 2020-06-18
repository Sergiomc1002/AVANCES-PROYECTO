#include "syscall.h"
#include "synch.h"

void SimpleThread(int,Barrier&);

int main( int argc, char * argv[] ) {
	Barrier b("miBarrera",2);
	Fork(SimpleThread);

	SimpleThread(1);

	
    Write("Main  \n", 7, 1);
    //Write(argc, 4, 1);
    //Write(argv, 4, 1);
}


void SimpleThread(int num, Barrier & b)
{
    if (num == 1) {
		for (num = 0; num < 50; num++) {
			Write("HOLAA ", 6, 1);
			//Yield();
		}
    }

    else {
	for (num = 0; num < 5; num++) {
		Write("2. holaa\n", 10, 1);
		//Yield();
	}
    }
	Write("LLEGO HILO",10,1);
	b.Wait();
	
	
    Write("Fin de\n", 7, 1);
}

