output: test.o http_parser.o
	g++ test.o http_parser.o -o test

test.o: test.cc
	g++ -c test.cc

http_parser.o: http_parser.cc http_parser.h
	g++ -c http_parser.cc http_parser.h

clean:
	rm *.o test