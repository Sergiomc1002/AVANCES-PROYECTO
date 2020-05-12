//PROTOTIPO DE COMO LEER LO QUE NOS MANDA EL SERVIDOR. DONDE VIENE EL HEADER SEGUIDO DE 1 ESPACIO, SEGUIDO DE LOS DATOS QUE MANDA. 
//AÚN NO ESTA COMPILADO. 

   char* response = (char*)calloc(512, sizeof(char));
	int bytes_read = 0; 
	int content_length = 512; 		//la longitud temporal antes de leer el header. 
	int read_status = -1; 
	bool b_exit = false; 
	int start_data = -1; 			//la posición donde comienzan los datos reales, sin el header. 
	char* data_from_server; 		//se va a inicializar posteriormente una vez se sepa cuantos bytes me esta mandando el server. 
	int index_data_from_server = 0; //para saber por donde vamos escribiendo. 
	
	while(bytes_read < content_length && !b_exit) {
		if (bytes_read != 0) {	//de la segunda iteración a la ultima, content_length ya tiene el valor real, por lo que solo entra si queda pendiente por leer. 
			memset(response, 0, 512 * sizeof(char));	//se limpia el buffer. 
			read_status = s.Read(response, 512);
			int len_current_read = strlen(response);	//siempre va a leer 512 bytes, a menos que sea la ultima leida, ahí solo sería lo que queda pendiente. 
			strcpy(data_from_server+index_data_from_server, response); 
			index_data_from_server+= len_current_read; 
			bytes_read+= len_current_read; 
		}
		else {			//es la primera iteración, tengo que leer el header. 
			read_status = s.Read(response, 512);
			if (read_status != -1) {
				content_length = get_file_size(response);	//se averigua ya el verdadero content_length, para saber si hay que seguir leyendo o no. 
				data_from_server = (char*)calloc(content_length,sizeof(char)); //se reserva memoria suficiente para guardar todo lo que manda el server. 
				bytes_read+= read_status; 
				int counter_data = 0; 
				while(counter_data < content_length) {	//buscar en que posicion del char* comienza la página html. 
					if (response[counter_data] == '\\' && response[counter_data+1] == 'n' && response[counter_data+2] == '\\' && response[counter_data+3] == 'n') {
						//si hay 2. \n seguidos. es decir, a partir de ahí comienza el archivo html. 
						start_data == counter_data+4; //los datos comienzan despues del ultimo \n. 
					}
					++counter_data; 
				}		//al finalizar el while, queda en start_data la posicion donde comienza la página html. 
				
				strcpy(data_from_server, response+start_data);	//copie todo lo que viene en el primer read, pero desde responde+start_data, sin el header. 
				index_data_from_server = strlen(response) - start_data; //necesito este index, para el siguiente copiado, y no caerle a los datos ya leidos. 
			}
			else {
				b_exit = true; 
			}
		}
	}
