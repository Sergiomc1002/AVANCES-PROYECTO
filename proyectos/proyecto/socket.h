#ifndef Socket_h
#define Socket_h

class Socket{

    public:
        Socket( char, bool = false );
        Socket( int );
        ~Socket();
        int Connect( char *, int );
        int Connect( char *, char * );
        int Read( char *, int len,  int id_socket);	
        int Read(char*, int len); 
        int Write( char *, int id_socket, int);		//para el server escribirle al cliente. 
        int Write(char*); 						//para el cliente escribirle al server. 
        int Listen( int );
        int Bind( int, int);
        Socket* Accept();
        int Accept(struct sockaddr_in* client_socket);	
        int Accept(struct sockaddr_in6* client_socket_ipv6); 
        void SetIDSocket( int );
        int Shutdown(int client_id); 
        int Shutdown(); 
        int recvFrom(void* buffer, int len_bufer, void* sockaddr); 
        int sendTo(void* msg, int len_msg, void* sockaddr); 
        
        
    private:
        int id;
        bool ipv6;
        int client_id; 
};

#endif

