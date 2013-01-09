ProWGen: request.o stream.o main.o stack.o 
	g++ -o ProWGen main.o request.o  stack.o stream.o -lm

request.o: request.cc request.h 
	gcc -c request.cc

stack.o: stack.cc stack.h
	gcc -c stack.cc

stream.o: stream.cc stream.h  stack.h 
	gcc -c stream.cc

main.o: main.cc stream.h
	gcc  -c main.cc


clean:
	rm -f ProWGen main.o request.o stack.o stream.o 