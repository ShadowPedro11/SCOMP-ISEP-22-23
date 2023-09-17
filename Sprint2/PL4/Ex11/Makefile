Server: Server.o
	gcc -g -Wall Server.o -o Server -lpthread -lrt
	
Server.o: Server.c
	gcc -g -Wall -c Server.c -o Server.o
	
Consumer: Consumer.o
	gcc -g -Wall Consumer.o -o Consumer -lpthread -lrt
	
Consumer.o: Consumer.c
	gcc -g -Wall -c Consumer.c -o Consumer.o
	
clean:
	rm *.o

run: Server Consumer
	./Server
	./Consumer

