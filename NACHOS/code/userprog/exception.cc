// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>



void returnFromSystemCall() {

		machine->WriteRegister( PrevPCReg, machine->ReadRegister( PCReg ) );
		machine->WriteRegister( PCReg, machine->ReadRegister( NextPCReg ) );
		machine->WriteRegister( NextPCReg, machine->ReadRegister( NextPCReg ) + 4 );

}       // returnFromSystemCall


/*
 *  System call interface: Halt()
 */
void NachOS_Halt() {		// System call 0

	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();

}



Semaphore* find_my_sem(ListElement<Semaphore*>* head, int my_id) {
	char* id = (char*)calloc(20, sizeof(char));
	sprintf(id,"%d",my_id); 
	
	ListElement<Semaphore*>* c_node = head; 
	
	Semaphore* my_sem = NULL;  
	
	bool found = false; 
	while (c_node != NULL && !found) {
		Semaphore* sem = c_node->item; 
		char* c_name = sem->getName(); 
		if (strcmp(id, c_name) == 0) {
			found = true; 
			my_sem = sem; 
		}
		else {
			c_node = c_node->next; 
		}
	}
	
	return my_sem; 
}


int is_someone_waiting_for_me(ListElement<father_son_t*>* head, int my_id) {	//averigua si alguien espera, de ser vrdd, retorna el id de quien espera. 
	int who = -1; 
	
	ListElement<father_son_t*>* c_node = head; 
	
	while (c_node != NULL && who == -1) {
		father_son_t* c_tuple = c_node->item; 
		if (my_id == c_tuple->son_pid) {
			who = c_tuple->father_pid; 
		}
		else {
			c_node = c_node->next; 
		}
	}

	return who; 
}



/*
 *  System call interface: void Exit( int )
 */


	// BORRAR DEL BITMAP EL ADDRES SPACE DEL HILO
	
    // CAMBIA EL ESTATUS DEL THREAD (SI DA 0, SE SALIO BIEN )
    // currentThread->setStatus(exitStatus);

    // SI EL PADRE DEL HILO ESTA DORMIDO, LO DESPIERTA
    //if((currentThread ->getParent()) != NULL && (currentThread->getParent()-> getStatus() == BLOCKED)){
		//scheduler->ReadyToRun(currentThread->getParent()); //LE DICE AL SCHEDULER QUE DESPIERTE AL PADRE
		
    //}
    
    //tengo que buscar en waiting_list si yo estoy ahí, sacar al padre y hacerle signal.
    //
	


void NachOS_Exit() {		// System call 1
	ThreadStatus exitStatus = (ThreadStatus)machine->ReadRegister(4);
	int threadId = currentThread->get_pid();

	printf("soy el hilo/proceso : %d y estoy saliendo \n", currentThread->get_pid()); 
	
	if (!currentThread->space->am_I_process()) {			//si no soy un proceso, soy un hilo. 
		ListElement<father_son_t*>* c_node = waiting_list->head(); 
		int who = is_someone_waiting_for_me(c_node, currentThread->get_pid()); 
		
		if (-1 != who) {
			printf("soy el hilo : %d y me esta esperando el hilo/proceso : %d \n", currentThread->get_pid(), who); 
			printf("liberandolo ... \n"); 
			Semaphore* sem = find_my_sem(process_threads->head(), who); 
			sem->V(); 
		}
		else {
			//nadie me esta esperando. 
		}
	}
	else {													//por ser proceso, no tengo que esperar a nadie, ni siquiera busco. 
		//nadie me puede esperar. 
	}

    // FINALIZA EL THREAD
	currentThread->space->deleteAddrspace();
	currentThread->Finish();
}


/*
 *  System call interface: SpaceId Exec( char * )
 */
 
 //quien le manda el char* ??
void NachOS_Exec() {		// System call 2
	
	int data = machine->ReadRegister(4);			//yo se que es un char*
	
	int* buffer = (int*)calloc(1, sizeof(int)); 
	
	int data_capacity = 100; 
	char* all_data = (char*)calloc(data_capacity, sizeof(char)); 
	int index_all_data = 0; 
	
	machine->ReadMem(data, 1, buffer);
	
	char c_data[1]; 
	c_data[0] = *(char*)buffer;

	 
	while(c_data[0] != '\0' && index_all_data < data_capacity) {
		all_data[index_all_data] = c_data[0]; 
		
		++data; 
		++index_all_data; 
		machine->ReadMem(data, 1, buffer); 
		c_data[0] = *(char*)buffer; 
	} 

	if (c_data[0] == '\0' && index_all_data < data_capacity) {	//se pudo leer todo en el buffer de capacidad :  "data_capacity"
		all_data[index_all_data] = c_data[0]; 
		OpenFile * executable = fileSystem->Open(all_data); 
		AddrSpace * addr_space = new AddrSpace(executable);
		currentThread->space = addr_space; 
		addr_space->InitRegisters(); 
		addr_space->RestoreState(); 
		free(all_data);
		free(c_data);
		machine->Run();   
	}
	else {					//no hay suficiente espacio. 
		printf("no se pudo realizar exec por falta de memoria\n"); 
	}


//se supone que EXEC le tiene que pasar el id, a Join, pero no entiendo porque. no tiene mucho sentido creo. 
}

 
 
/*
 *  System call interface: int Join( SpaceId )
 */
  
/*
 * 
 * Francisco Arroyo:
En el "Join" tenemos la información de los dos hilos, como usted indica, "currentThread" que es el proceso padre P y en registro 4 el hilo por el 
* que debo esperar H.  Entonces en el momento que el proceso llama a "Join" debe crear una estructura de datos para almacenar 
* esta relación "P espera por H", luego el proceso P debe esperar por un semáforo, por ejemplo.

Cuando el proceso H termine, llamando a "Exit" verifica la estructura de datos para comprobar si hay alguno esperando por él y, 
* en caso positivo, le hace "Signal" al semáforo para que el otro proceso P puede seguir su ejecución.  Recuerde también que "Exit" 
* recibe un parámetro que se debe entregar al proceso P como retorno del "Join".

Le llega lo que usted devuelva en "Exec".

Recuerdo que NO es buena idea pasar punteros de estructuras a los programas de usuario, entonces sería recomendable que "Exec" devuelva un número, 
* por ejemplo, un índice de una tabla de los procesos que hacen "Join". 
 * */


void NachOS_Join() {		// System call 3

int  child_pid = machine->ReadRegister(4); 

int my_pid = currentThread->get_pid(); 

father_son_t* father_son = (father_son_t*)calloc(1,sizeof(father_son_t)); 
father_son->son_pid =  child_pid; 
father_son->father_pid = my_pid; 

waiting_list->Append(father_son); 

Semaphore* my_sem; 		//se lo puedo poner de atributo a thread asi lo accedo mas facil. 
char* id = (char*)calloc(20, sizeof(char));

ListElement<Semaphore*>* c_node = process_threads->head();  
my_sem = find_my_sem(c_node, my_pid); 
my_sem->setValue(my_sem->getValue()-1);			//el semaforo al crearse tiene 1 de valor, ahora quiero esperar 1 hilo, entonces lo decremento.  

printf("soy el hilo : %s - entrando al semaforo\n", my_sem->getName()); 

my_sem->P(); 									//voy a esperar y me tengo que quedar en el semaforo, hasta que alguien me haga signal. 

}


/*
 *  System call interface: void Create( char * )
 */
void NachOS_Create() {		// System call 4
}


/*
 *  System call interface: OpenFileId Open( char * )
 */
void NachOS_Open() {		// System call 5
}


/*
 *  System call interface: OpenFileId Write( char *, int, OpenFileId )
											//reg4 , reg5, reg6
 */
 
 
 
//NOTA PARA WRITE 
//El método ReadMem solo puede leer enteros, ver el tercer parámetro, por lo que se recomienda que las tiras de caracteres 
//se lean caracter por caracter (for) e irlas almacenando en una variable local, por ejemplo, char buffer[ 100 ];  
 
void NachOS_Write() {		// System call 6
	//char buffer;
//machine -> ReadRegister(X) // x = lee el registro 4(x) //devuelve un indice de la memoria de usuario 
//machine -> ReadMem(addres(registro X), cuantos caracteres leer (1), a donde se va a guardar ) //directorio machine, lee 1 byte a la vez, se usa para traer datos de la memori
		//Readmem(4,1,&buffer); se lee 1 caracter del registro 4  // 'a'
	
	int addr, count;
	int file = 0; 
   addr = machine->ReadRegister( 4 );
   count = machine->ReadRegister( 5 );
   
   file = machine->ReadRegister( 6 );
   
  //Semaphore* semaphore = new Semaphore("semaphore_write", 1); 
  
  //semaphore->P();
	switch(file) {
		case ConsoleOutput:
		   int c; 
		   for (int index = 0; index < count; ++index) { 
				machine->ReadMem( addr+index, 1, &c);
				printf("%c", c);
			}		
			break;
		
		case ConsoleInput:
			machine->WriteRegister( 2, -1 );		//registro 2, es de retorno, -1 de error, no se puede escribir en el Console input. 
		break; 
		
		case ConsoleError:
		
		break; 
		
		default:
			
			// Verify if the file is opened, if not return -1 in r2
			// Get the unix handle from our table for open files
			// Do the write to the already opened Unix file
			// Return the number of chars written to user, via r2		
		
		break; 
	}
	
	
	
   //semaphore->V();
   /*

	*/
	
/*
   int *buffer = (int*)calloc(count, sizeof(int)); 
  
   
   for (int index_data = 0; index_data < count; ++index_data) { 
		machine->ReadMem( addr+index_data, 1, buffer+index_data);
   }
	printf("%s\n", buffer);
	free(buffer); 
	
	*/	
	
	
}

//PENDIENTE - LO QUE FALTA DE WRITE. 
//void Write(
//        char * buffer = NULL;
//        int size = machine->ReadRegister( 5 );	// Read size to write

          // buffer = Read data from address given by user;
//        OpenFileId id = machine->ReadRegister( 6 );	// Read file descriptor

	// Need a semaphore to synchronize access to console
	// Console->P();
//	switch (id) {
//		case  ConsoleInput:	// User could not write to standard input
//			machine->WriteRegister( 2, -1 );
//			break;
//		case  ConsoleOutput:
//			buffer[ size ] = 0;
//			printf( "%s", buffer );
//		break;
//		case ConsoleError:	// This trick permits to write integers to console
//			printf( "%d\n", machine->ReadRegister( 4 ) );
//			break;
//		default:	// All other opened files
			// Verify if the file is opened, if not return -1 in r2
			// Get the unix handle from our table for open files
			// Do the write to the already opened Unix file
			// Return the number of chars written to user, via r2
//			break;

//	}
	// Update simulation stats, see details in Statistics class in machine/stats.cc
	// Console->V();

//}       // Nachos_Write




/*
 *  System call interface: OpenFileId Read( char *, int, OpenFileId )
 */
void NachOS_Read() {		// System call 7
}


/*
 *  System call interface: void Close( OpenFileId )
 */
void NachOS_Close() {		// System call 8
}


//void NachosForkThread( int p ) { // for 32 bits version
 void NachosForkThread( void * p ) { // for 64 bits version
    //printf("NachosForkThread\n");    
    //printf("ESTOY EN NACHOS_FORK_THREAD\n"); 
    
    AddrSpace *space;

    space= currentThread->space;
    space->InitRegisters();             // set the initial register values
    space->RestoreState();              // load page table register

// Set the return address for this thread to the same as the main thread
// This will lead this thread to call the exit system call and finish
    machine->WriteRegister( RetAddrReg, 4 );

    machine->WriteRegister( PCReg, (long) p );
    machine->WriteRegister( NextPCReg, (long) p + 4 );

    machine->Run();                     // jump to the user progam

    //printf("SALIENDO DE NACHOS_FORK_THREAD\n");    
    ASSERT(false);
}

/*
 *  System call interface: void Fork( void (*func)() )
 */
void NachOS_Fork() {		// System call 9
   
   
   DEBUG( 'u', "Entering Fork System call\n" );
   //printf("ESTOY EN NACHOS_FORK()\n "); 
   
   int id = currentThread->get_pid();
   char * myId = (char *)calloc(20,sizeof(char));
   sprintf(myId,"%d",id);
   Semaphore * sem = new Semaphore(myId,1);	//cada proceso/hilo que ejecuta fork, va a necesitar un semaforo, se usa solo si hace join. 
   
   process_threads->Append(sem); 			//lista de semaforos, cada semaforo tiene de nombre el mismo id del hilo. 
   
   Thread * newT = new Thread("Child");
      
	newT->space = new AddrSpace( currentThread->space );
	
   newT->Fork((VoidFunctionPtr)NachosForkThread, (void*)machine->ReadRegister(4));
   
   //printf("SALIENDO DE NACHOS_FORK()\n");
   
   //machine->WriteRegister(2,newT->get_pid());	//pasarle al padre el id del hijo. 
 
}



/*
The exact behavior of this function depends on the implementation, in particular on the mechanics of the OS scheduler in use and the
*  state of the system. For example, a first-in-first-out realtime scheduler (SCHED_FIFO in Linux) would suspend the current thread 
* and put it on the back of the queue of the same-priority threads that are ready to run (and if there are no other threads at the same priority,
*  yield has no effect). 
*/

/*
 *  System call interface: void Yield()
 */
void NachOS_Yield() {		// System call 10
	currentThread->Yield(); 					//creo que solo es hacer esto. 
}


/*
 *  System call interface: Sem_t SemCreate( int )
 */
void NachOS_SemCreate() {		// System call 11

}


/*
 *  System call interface: int SemDestroy( Sem_t )
 */
void NachOS_SemDestroy() {		// System call 12
}


/*
 *  System call interface: int SemSignal( Sem_t )
 */
void NachOS_SemSignal() {		// System call 13
}


/*
 *  System call interface: int SemWait( Sem_t )
 */
void NachOS_SemWait() {		// System call 14
}


/*
 *  System call interface: Lock_t LockCreate( int )
 */
void NachOS_LockCreate() {		// System call 15
}


/*
 *  System call interface: int LockDestroy( Lock_t )
 */
void NachOS_LockDestroy() {		// System call 16
}


/*
 *  System call interface: int LockAcquire( Lock_t )
 */
void NachOS_LockAcquire() {		// System call 17
}


/*
 *  System call interface: int LockRelease( Lock_t )
 */
void NachOS_LockRelease() {		// System call 18
}


/*
 *  System call interface: Cond_t LockCreate( int )
 */
void NachOS_CondCreate() {		// System call 19
}


/*
 *  System call interface: int CondDestroy( Cond_t )
 */
void NachOS_CondDestroy() {		// System call 20
}


/*
 *  System call interface: int CondSignal( Cond_t )
 */
void NachOS_CondSignal() {		// System call 21
}


/*
 *  System call interface: int CondWait( Cond_t )
 */
void NachOS_CondWait() {		// System call 22
}


/*
 *  System call interface: int CondBroadcast( Cond_t )
 */
void NachOS_CondBroadcast() {		// System call 23
}


/*
 *  System call interface: Socket_t Socket( int, int )
 */
void NachOS_Socket() {			// System call 30
}


/*
 *  System call interface: Socket_t Connect( char *, int )
 */
void NachOS_Connect() {		// System call 31
}


/*
 *  System call interface: int Bind( Socket_t, int )
 */
void NachOS_Bind() {		// System call 32
}


/*
 *  System call interface: int Listen( Socket_t, int )
 */
void NachOS_Listen() {		// System call 33
}


/*
 *  System call interface: int Accept( Socket_t )
 */
void NachOS_Accept() {		// System call 34
}


/*
 *  System call interface: int Shutdown( Socket_t, int )
 */
void NachOS_Shutdown() {	// System call 25
}


//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------





//PENDIENTE:
//Modificar el método ExceptionHandler de manera que podamos agregar los llamados al sistema que vayamos ocupando

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch ( which ) {

       case SyscallException:
          switch ( type ) {
             case SC_Halt:		// System call # 0
                NachOS_Halt();
                break;
             case SC_Exit:		// System call # 1
                NachOS_Exit();
                break;
             case SC_Exec:		// System call # 2
                NachOS_Exec();
                break;
             case SC_Join:		// System call # 3
                NachOS_Join();
                break;

             case SC_Create:		// System call # 4
                NachOS_Create();
                break;
             case SC_Open:		// System call # 5
                NachOS_Open();
                break;
             case SC_Read:		// System call # 6
                NachOS_Read();
                break;
             case SC_Write:		// System call # 7
                NachOS_Write();
                break;
             case SC_Close:		// System call # 8
                NachOS_Close();
                break;

             case SC_Fork:		// System call # 9
                NachOS_Fork();
                break;
             case SC_Yield:		// System call # 10
                NachOS_Yield();
                break;

             case SC_SemCreate:         // System call # 11
                NachOS_SemCreate();
                break;
             case SC_SemDestroy:        // System call # 12
                NachOS_SemDestroy();
                break;
             case SC_SemSignal:         // System call # 13
                NachOS_SemSignal();
                break;
             case SC_SemWait:           // System call # 14
                NachOS_SemWait();
                break;

             case SC_LckCreate:         // System call # 11
                NachOS_LockCreate();
                break;
             case SC_LckDestroy:        // System call # 12
                NachOS_LockDestroy();
                break;
             case SC_LckAcquire:         // System call # 13
                NachOS_LockAcquire();
                break;
             case SC_LckRelease:           // System call # 14
                NachOS_LockRelease();
                break;

             case SC_CondCreate:         // System call # 11
                NachOS_CondCreate();
                break;
             case SC_CondDestroy:        // System call # 12
                NachOS_CondDestroy();
                break;
             case SC_CondSignal:         // System call # 13
                NachOS_CondSignal();
                break;
             case SC_CondWait:           // System call # 14
                NachOS_CondWait();
                break;
             case SC_CondBroadcast:           // System call # 14
                NachOS_CondBroadcast();
                break;

             case SC_Socket:	// System call # 20
				NachOS_Socket();
               break;
             case SC_Connect:	// System call # 21
				NachOS_Connect();
               break;
             case SC_Bind:	// System call # 22
				NachOS_Bind();
               break;
             case SC_Listen:	// System call # 23
				NachOS_Listen();
               break;
             case SC_Accept:	// System call # 22
				NachOS_Accept();
               break;
             case SC_Shutdown:	// System call # 23
				NachOS_Shutdown();
               break;

             default:
                printf("Unexpected syscall exception %d\n", type );
                ASSERT( false );
                break;
          }
         returnFromSystemCall();
          break;

       case PageFaultException: {
          break;
       }

       case ReadOnlyException:
          printf( "Read Only exception (%d)\n", which );
          ASSERT( false );
          break;

       case BusErrorException:
          printf( "Bus error exception (%d)\n", which );
          ASSERT( false );
          break;

       case AddressErrorException:
          printf( "Address error exception (%d)\n", which );
          ASSERT( false );
          break;

       case OverflowException:
          printf( "Overflow exception (%d)\n", which );
          ASSERT( false );
          break;

       case IllegalInstrException:
          printf( "Ilegal instruction exception (%d)\n", which );
          ASSERT( false );
          break;

       default:
          printf( "Unexpected exception %d\n", which );
          ASSERT( false );
          break;
    }

}
