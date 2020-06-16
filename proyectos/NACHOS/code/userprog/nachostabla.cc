#include "nachostabla.h"


void NachosOpenFilesTable::NachosOpenFilesTable() {
	this->open_files = (int*)calloc(MAX_FILES, sizeof(int)); 
	this->usage = 1;			//siempre hay como minimo un hilo/proceso usandola cuando se crea.  
	this->open_files_map = new BitMap(MAX_FILES); 
	this->max_length = MAX_FILES;
	this->current_length = 0; 
}


void NachosOpenFilesTable::~NachosOpenFilesTable() {
	free(this->open_files); 
	delete this->open_files_map; 
}

int NachosOpenFilesTable::open(int unix_handle) {
	if (this->current_length == this->max_length) {
	 
		int * new_open_files = (int*)calloc(this->length*2, sizeof(int)); 
		BitMap* new_open_files_map = new BitMap(this->length*2);
		
		unsigned int *old_map = this->open_files_map->get_map(); 
		unsigned int *new_map = new_open_files_map->get_map(); 
		
		for (int index_open_files = 0; index_open_files < this->length; ++index_open_files) {
			*(new_open_files+index_open_files) = *(this->open_files+index_open_files);
			*(new_map+index_open_files) = *(old_map+index_open_files);
		}
		
		this->current_length = this->current_length*2;
		
		int *temporal_pointer = this->open_files; 
		this->open_files = new_open_files; 
		free(temporal_pointer);
		
		temporal_pointer = this->open_files_map; 
		this->open_files_map = new_open_files_map;
		delete temporal_pointer;  
		
	}
	
		*(this->open_files+this->open_files_map->Find()) = unix_handle;
		++this->current_length;  	
}

int NachosOpenFilesTable::close(int nachos_handle) {
	bool found = false; 
	int index_open_files = 0;
	
	while (index_open_files < this->max_length && !found) {
		if (this->open_files_map->Test(index_open_files) && *(this->open_files+index_open_files) == nachos_handle) {
			this->open_files_map->Clear(index_open_files);
			--this->current_length; 
			found = true;  
		}
		++index_open_files; 
	}
	
}

int NachosOpenFilesTable::get_current_index() {
	return this->current_index; 
}


bool is_opened (int nachos_handle) {
	bool found = false; 
	int index_open_files = 0; 
	
		while (index_open_files < this->max_length && !found) {
			if (this->open_files_map->Test(index_open_files) && *(this->open_files+index_open_files) == nachos_handle) {
				found = true; 
			}
			++index_open_files; 
		}
	return found; 	
}

int get_unix_handle(int nachos_handle) {
	bool found = false; 
	int index_open_files = 0; 
	
		while (index_open_files < this->max_length && !found) {
			if (this->open_files_map->Test(index_open_files) && *(this->open_files+index_open_files) == nachos_handle) {
				found = true; 
				--index_open_files;
			}
			++index_open_files; 
		}
		
	
	return index_open_files; 	
}


void add_thread() {
	++this->usage; 
}


void del_thread() {
	if (this->usage == 1) {			//soy el ultimo thread, tengo que borrar todo. 
		
	}
	else {
		--this->usage; 
	}
}


void print() {
	
	
}
