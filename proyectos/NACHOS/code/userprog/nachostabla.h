

/*Los métodos "addThread" y "delThread" se utilizan para que todos los hilos de un programa de usuario utilicen la misma tabla, 
 * lo que hacemos es que cada vez que se crea un hilo, incrementamos la variable de instancia "usage", de la misma manera que cuando 
 * los hilos vayan terminando, se decrementa; si ya es el último hilo, entonces éste cerraría todos los archivos abiertos indicados por nuestra tabla. 
 * La idea es que un hilo no cierre los archivos que pueden emplear otros hilos. 
*/
#ifndef NACHOS_TABLA
#define NACHOS_TABLA


#define MAX_FILES 20;

class NachosOpenFilesTable {
  public:
    NachosOpenFilesTable();       // Initialize 
    ~NachosOpenFilesTable();      // De-allocate
    
    int open( int unix_handle ); // Register the file handle
    int close( int nachos_handle );      // Unregister the file handle
    bool is_opened( int nachos_handle );
    int get_unix_handle( int nachos_handle );
    void add_thread();		// ESTO CREO QUE SE TENDRIA QUE HACER EL EL FORK()
    void del_thread();		// 

    void print();               // Print contents
    int get_current_index(); 
    
  private:
    int * open_files;		// A vector with user opened files
    BitMap * open_files_map;	// A bitmap to control our vector
    int usage;			// How many threads are using this table
	int current_length;
	int max_length;  
};

#endif
