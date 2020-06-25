output: client server balanceador

server: server.o socket.o http_parser.o
	g++ server.o socket.o http_parser.o -o server -lpthread
	
balanceador: balanceador.o socket.o http_parser.o
	g++ balanceador.o socket.o http_parser.o -o balanceador -lpthread

client: client.o socket.o http_parser.o
	g++ client.o socket.o http_parser.o -o client

server.o: server.cpp
	g++ -c server.cpp 

balanceador.o: balanceador.cpp
	g++ -c balanceador.cpp

client.o: client.cpp
	g++ -c client.cpp 

socket.o: socket.cpp socket.h
	g++ -c socket.cpp


http_parser.o: http_parser.cc http_parser.h
	g++ -c http_parser.cc

clean:
	rm *.o  client server balanceador
