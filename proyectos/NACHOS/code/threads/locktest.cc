#include "synch.h"

void SimpleThread3(void * arg);

struct LTA // Lock Test Arguments
{
    Lock * l;
    Barrier * b;
};

int lockTest() {
	Lock l("myLock");
    Barrier b("barrier", 2);

	Thread * child = new Thread("Child2");

    
    LTA btaC; // lock test arg child
    btaC.l = &l;
    btaC.b = &b;
    
    child->Fork((VoidFunctionPtr)SimpleThread3, (void *) &btaC);

    l.Acquire();

    for(int i = 1; i <= 5; i++) 
    {
        printf("Padre %d\n", i);
    }

    l.Release();

    printf("Padre: Termine\n");

    b.Wait();
}


void SimpleThread3(void * arg)
{
    LTA * args = (LTA *) arg;

    args->l->Acquire();

    for(int i = 1; i <= 5; i++) 
    {
        printf("Hijo %d\n", i);
    }

    args->l->Release();

    args->b->Wait();
    printf("Hijo: Termine\n");

    args->b->Wait();
}

