#include "synch.h"

void SimpleThread2(void * arg);


int barrierTest() {
	Barrier b("miBarrera",2);

	Thread * child = new Thread("Child");
    child->Fork((VoidFunctionPtr)SimpleThread2, (void *) &b);

    b.Wait();

    printf("Padre: Termine\n");
}


void SimpleThread2(void * arg)
{
    Barrier * b = (Barrier *) arg;

    for(int i = 0; i < 10; i++) 
    {
        printf("Termine: Hola\n");
    }

    b->Wait();

    printf("Hijo: Termine\n");
}

