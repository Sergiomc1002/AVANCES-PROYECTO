#define FILE_NAME_SIZE 120
#define REQUEST_HEADER_SIZE 120
#define RESPONSE_HEADER_SIZE 120

extern char* get_file_name(char* header);

extern int get_file_size(char* response_header);

extern char* make_request_header(char* filename);

extern char* make_response_header(char* filename);

extern char* get_extension_file(char* response_header);
