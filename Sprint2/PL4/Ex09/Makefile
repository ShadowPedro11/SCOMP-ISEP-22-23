main: main.o
	gcc -g -Wall main.o -o main -lpthread -lrt
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o
	
clean:
	rm *.o

run: main
	./main 5 100 us
	./main 5 500 us
	./main 5 1 ms
	./main 5 10 ms
	./main 5 50 ms
	./main 5 500 ms
	./main 5 1 s
