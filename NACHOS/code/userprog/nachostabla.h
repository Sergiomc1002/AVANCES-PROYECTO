

/*Los métodos "addThread" y "delThread" se utilizan para que todos los hilos de un programa de usuario utilicen la misma tabla, 
 * lo que hacemos es que cada vez que se crea un hilo, incrementamos la variable de instancia "usage", de la misma manera que cuando 
 * los hilos vayan terminando, se decrementa; si ya es el último hilo, entonces éste cerraría todos los archivos abiertos indicados por nuestra tabla. 
 * La idea es que un hilo no cierre los archivos que pueden emplear otros hilos. 
*/
#ifndef NACHOS_TABLA
#define NACHOS_TABLA

class NachosOpenFilesTable {
  public:
    NachosOpenFilesTable();       // Initialize 
    ~NachosOpenFilesTable();      // De-allocate
    
    int open( int unix_handle ); // Register the file handle
    int close( int nachos_handle );      // Unregister the file handle
    bool is_opened( int nachos_handle );
    int get_unix_handle( int nachos_handle );
    void add_thread();		// If a user thread is using this table, add it
    void del_thread();		// If a user thread is using this table, delete it

    void print();               // Print contents
    
  private:
    int * openFiles;		// A vector with user opened files
    BitMap * openFilesMap;	// A bitmap to control our vector
    int usage;			// How many threads are using this table

};

#endif
