output: client server test

server: server.o socket.o http_parser.o
	g++ server.o socket.o http_parser.o -o server -lpthread
	
client: client.o socket.o http_parser.o
	g++ client.o socket.o http_parser.o -o client

test: test.o http_parser.o
	g++ test.o http_parser.o -o test

server.o: server.cpp
	g++ -c server.cpp 

client.o: client.cpp
	g++ -c client.cpp 

socket.o: socket.cpp socket.h
	g++ -c socket.cpp

test.o: test.cc
	g++ -c test.cc

http_parser.o: http_parser.cc http_parser.h
	g++ -c http_parser.cc

clean:
	rm *.o test client server
