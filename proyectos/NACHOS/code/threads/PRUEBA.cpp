
#include <unistd.h>
#include "copyright.h"
#include "system.h"
#include "dinningph.h"
typedef struct{
	int compartido;
}mierda;

void simpleThread(void * argv){
    //printf("SOY UN HILO %s \n",(int)argv);
	mierda * desecho = (mierda*)argv;
	printf("DESEHO: %d\n",desecho->compartido);
}



void TEST(){
	mierda caca;
	caca.compartido = 234;
	for(int i = 0; i<4; ++i){
	  char* threadname = new char[100];
      sprintf(threadname, " %d", i);
      Thread* newThread = new Thread (threadname);
      newThread->Fork (simpleThread, (void*)&caca);
	  //printf("COMPARTIDO 1: %d\n",compartido);
	}
	int x = 0;
	while( x < 10000){
		x++;
	}
	//printf("COMPARTIDO: %d\n",compartido);
	
}