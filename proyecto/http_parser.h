#define FILE_NAME_SIZE 120
#define REQUEST_HEADER_SIZE 120
#define RESPONSE_HEADER_SIZE 120
#define PORT_TO_CONNECT 80

typedef struct {
	char* ip_address; 
	char* filename; 
	char* port; 
	int server_port; 
	
}data_arguments_t; 

extern void set_initial_values(data_arguments_t *data_arguments, char* request); 

extern void free_initial_values(data_arguments_t* data_arguments); 

extern char* get_file_name(char* header);

extern int get_file_size(char* response_header);

extern char* make_request_header(char* filename);

extern char* make_response_header(char* filename, int content_length);

extern char* get_file_extension(char* response_header);

extern int get_index_start_data(char* response, int read_status); 

extern int get_http_status(char* response_header, int read_status); 

extern char* get_extension_filename(char* filename); 
