#include "syscall.h"

void SimpleThread(int);

int main( int argc, char * argv[] ) {
	
	//int my_id = currentThread->get_pid(); 
	
   

	//FORK deberia retonarme el id, para saber a quien tengo que esperar. 

	//char msg[100]; 
	//snprintf(msg, sizeof(msg), "id del hijo : %d \n", child_id);



	Fork(SimpleThread);

	//int child_id = machine->ReadRegister(2); 

	Join(7); 


	//Join(child_id); 		//primero se iria a dormir, y luego ejecutaria SimpleThread, por lo que el hijo deberia imprimir primero. 
	

	SimpleThread(1);

	
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

