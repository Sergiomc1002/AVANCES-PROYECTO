// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//

#include <unistd.h>
#include "copyright.h"
#include "system.h"
#include "dinningph.h"
#include <time.h>

typedef struct {
	int variable; 
	Semaphore* semaphore;  
	Mutex* mutex; 
	Lock * lock; 
} shared_data_t; 


typedef struct {
	shared_data_t* shared_data; 
	int thread_id; 
}thread_data_t; 


typedef struct {
	Lock* lock; 
	int iterations; 
	int length_buffer; 
	int* buffer; 
	int* items; 
	Condition* producer_condition; 
	Condition* consumer_condition; 
	
} producer_consumer_data_t;


void producer(void* data) {
	producer_consumer_data_t* producer_consumer_data = (producer_consumer_data_t*)data; 
	Lock* lock = producer_consumer_data->lock; 
	int iterations = producer_consumer_data->iterations; 
	int length_buffer = producer_consumer_data->length_buffer; 
	int* buffer = producer_consumer_data->buffer; 
	Condition* producer_condition = producer_consumer_data->producer_condition; 
	Condition* consumer_condition = producer_consumer_data->consumer_condition; 
	int *items = producer_consumer_data->items; 
	
	for (int iteration = 0; iteration < iterations; ++iteration) {
		
		for (int item_produced = 0; item_produced < length_buffer; ++item_produced ) {
		
			lock->Acquire(); 
		
				while (*items == 10) {
					printf("producer : voy a entrar al wait\n");
					producer_condition->Wait(lock);
					printf("producer : sali del wait\n"); 
				}
				int item = rand() % 100; 
				printf("soy el productor(iteracion : %d) y estoy produciendo el item : %d \n", iteration, item); 
				buffer[*items] = item; 
				(*items)++;
			
			consumer_condition->Signal(lock);
			lock->Release();
		
		} 
	}
		
}



void consumer(void* data) {
	producer_consumer_data_t* producer_consumer_data = (producer_consumer_data_t*)data; 
	Lock* lock = producer_consumer_data->lock; 
	int iterations = producer_consumer_data->iterations; 
	int length_buffer = producer_consumer_data->length_buffer; 
	int* buffer = producer_consumer_data->buffer; 
	Condition* producer_condition = producer_consumer_data->producer_condition; 
	Condition* consumer_condition = producer_consumer_data->consumer_condition; 	
	int *items = producer_consumer_data->items; 	
	
	for (int iteration = 0; iteration < iterations; ++iteration) {
		for (int item_consumed = 0; item_consumed < length_buffer; ++item_consumed) {
		
			lock->Acquire(); 
		
				while(*items == 0) {
					printf("consumer : voy a entrar al wait\n"); 
					consumer_condition->Wait(lock);
					printf("consumer : sali del wait\n");  
				}
				int item = buffer[*items]; 
				printf("soy el consumidor(iteracion : %d) y estoy consumiendo el item : %d \n" ,iteration, item);
				(*items)--; 


			producer_condition->Signal(lock); 
			consumer_condition->Down(lock); 			//no es necesario, pero lo hace más eficiente. 
			lock->Release();
		} 
	}
}


void run (void* data) {
	
	thread_data_t* thread_data = (thread_data_t*)data; 

	int thread_id = thread_data->thread_id; 
	
	shared_data_t* shared_data = thread_data->shared_data; 
	
	Semaphore* semaphore = shared_data->semaphore; 
	Mutex* mutex = shared_data->mutex;
	Lock* lock = shared_data->lock; 

	//zona critica. 
	
	if (thread_id == 0) {
		printf("soy el thread : %d y me voy a dormir\n", thread_id);
	}


	//mutex->Lock(); 
	//semaphore->P();
	lock->Acquire(); 
	
	if (thread_id == 0) { 
		printf("soy el thread : %d y ya me despertaron \n", thread_id); 
	}
	
	printf("soy el thread : %d y estoy entrando a la zona critica\n", thread_id); 
	
	printf("valor actual de la variable : %d\n", shared_data->variable); 
	
	shared_data->variable++; 
	
	printf("valor despue de modificar la variable : %d\n", shared_data->variable); 
	//mutex->Unlock(); 
	//semaphore->V(); 
	lock->Release(); 
}



void awake_thread(void* data) {

	thread_data_t* thread_data = (thread_data_t*)data; 

	int thread_id = thread_data->thread_id; 
	
	shared_data_t* shared_data = thread_data->shared_data; 
	
	Semaphore* semaphore = shared_data->semaphore; 
	Mutex* mutex = shared_data->mutex;	
	Lock* lock = shared_data->lock; 
	
	printf("soy el thread : %d y voy a abilitar el semaforo \n", thread_id); 
	//mutex->Unlock(); 
	//semaphore->V();
	lock->Release();  
}




void test_sema_mutex_lock() {
	
	printf("estoy entrando\n"); 
	
	shared_data_t *shared_data = (shared_data_t*)calloc(1,sizeof(shared_data_t)); 
	shared_data->variable = 0; 
	
	//Mutex *mutex = new Mutex("my_mutex"); 
	Lock *lock = new Lock("my_lock"); 
	
	Semaphore *semaphore = new Semaphore("my_semaphore", 0); 
	shared_data->semaphore = semaphore; 
	//shared_data->mutex = mutex; 
	
	shared_data->lock = lock; 
	
	thread_data_t* thread_data = (thread_data_t*)calloc(3, sizeof(thread_data_t)); 
	
	for (int i = 0; i < 2; ++i) {
			
			thread_data[i].shared_data = shared_data; 
			thread_data[i].thread_id = i; 
			
			Thread *my_thread = new Thread("modificador"); 
			my_thread->Fork(run, (void*)&thread_data[i]);
	}
	
	
		thread_data[2].shared_data = shared_data; 
		thread_data[2].thread_id = 2; 
		
		Thread* new_thread = new Thread("despertador"); 
		new_thread->Fork(awake_thread, (void*)&thread_data[2]); 
	
	printf("ya sali\n"); 
	
}




void test_producer_consumer() {
	
	producer_consumer_data_t* producer_consumer_data = (producer_consumer_data_t*)calloc(1, sizeof(producer_consumer_data_t));
	Lock* lock = new Lock("shared_lock"); 
	Condition* producer_condition = new Condition("producer_condition"); 
	Condition* consumer_condition = new Condition("consumer_condition");
	
	producer_consumer_data->lock = lock; 
	producer_consumer_data->producer_condition = producer_condition; 
	producer_consumer_data->consumer_condition = consumer_condition; 
	
	
	int iterations = 2; 
	int length_buffer = 10; 
	int* buffer = (int*)calloc(length_buffer, sizeof(int)); 
	int *items = (int*)calloc(1, sizeof(int)); 
	
	producer_consumer_data->items = items; 
	
	producer_consumer_data->iterations = iterations; 
	producer_consumer_data->length_buffer = length_buffer; 
	producer_consumer_data->buffer = buffer; 
	
	Thread* producer_thread = new Thread("Producer");
	Thread* consumer_thread = new Thread("Consumer");
	
	producer_thread->Fork(producer, (void*)producer_consumer_data);   
	consumer_thread->Fork(consumer, (void*)producer_consumer_data);
}




void test_dinning() {
    //Thread * Ph;

    //DEBUG('t', "Entering SimpleTest");

/*
    dp = new DinningPh();

    for ( long k = 0; k < 5; k++ ) {
        Ph = new Thread( "dp" );
        Ph->Fork( Philo, (void *) k );
    }

    return;
*/



	/*
    for ( int k=1; k<5; k++) {
      char* threadname = new char[100];
      sprintf(threadname, "Hilo %d", k);
      Thread* newThread = new Thread (threadname);
      newThread->Fork (SimpleThread, (void*)threadname);
    }
    
    SimpleThread( (void*)"Hilo 0");
	
	*/	
}





void
SimpleThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread %s looped %d times\n", threadName, num);
	//interrupt->SetLevel(oldLevel);
        //currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);
}


//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
	//test_sema_mutex_lock(); 
	
	test_producer_consumer(); 
	
}



/*
DinningPh * dp;

void Philo( void * p ) {

    int eats, thinks;
    long who = (long) p;

    currentThread->Yield();

    for ( int i = 0; i < 10; i++ ) {

        printf(" Philosopher %ld will try to pickup sticks\n", who + 1);

        dp->pickup( who );
        dp->print();
        eats = Random() % 6;

        currentThread->Yield();
        sleep( eats );

        dp->putdown( who );

        thinks = Random() % 6;
        currentThread->Yield();
        sleep( thinks );
    }

}
*/

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------


