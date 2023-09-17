#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>

#define BUFFER_SIZE 10
#define VALUES 30

typedef struct {
    int buffer[BUFFER_SIZE];
} CircularBuffer;



int main(int argc, char *argv[]){
	int fd, data_size, trunk, fd_parent_child[2], fd_child_parent[2];

    if (pipe(fd_parent_child) == -1) {
        perror("Pipe1 failed!\n");
        return 1;
    }

    if (pipe(fd_child_parent) == -1) {
        perror("Pipe2 failed!\n");
        return 1;
    }

	CircularBuffer *ptr;
	
	data_size = sizeof(CircularBuffer);
	
	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it to the size of CircularBuffer struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);												// Truncate the shared memory segment to the size of CircularBuffer 
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (CircularBuffer *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
	for (int i = 0; i < BUFFER_SIZE; i++) {
        ptr->buffer[i] = 0;
    }
    
	pid_t pid = fork();													// Create new proccess
	
	if(pid < 0){														//Check creation of child process 
		perror("Error creating process!");
		return 1;
	}
	
	if(pid > 0){	
		//Father-Producer

		close(fd_parent_child[0]);	//Close read
		close(fd_child_parent[1]);	//Close write

		char aux[10];
		char aux2;
		write(fd_parent_child[1], aux, 10);

		int j = 0;
		for(int i = 0; i<30;i++){										//For cycle to exchenge 30 values
			
			while(read(fd_child_parent[0], &aux2, sizeof(char)) == 0);	// wait until the buffer has space for the producer to write 
			j++;
			int position = (i%BUFFER_SIZE);								// found position on array to write
			
			while(ptr->buffer[position] != 0);							// check the position to write
			ptr->buffer[position] = i;									// write 
			
			printf("Producer | Item: %d\n",i);							// print the action
			
			write(fd_child_parent[1], &aux2, sizeof(char));				// Tell the consumer that the buffer as a value available to be read
			j--;
		}

		close(fd_parent_child[1]);
		close(fd_child_parent[0]);
			
	}else{
		//Child-Consumer

		close(fd_parent_child[1]);
		close(fd_child_parent[0]);

		char aux[10] = "0000000000";
		char aux2;
		write(fd_child_parent[1], aux, sizeof(10));

		int j = 0;

		for(int i = 0; i<30;i++){										//For cycle to exchenge 30 values
			while(read(fd_parent_child[0], &aux2, sizeof(char)) == 0);				//wait while check if have values available to read 
			int position = (i%BUFFER_SIZE);								// found position on array to write
			
			printf("Consumer | Item: %d\n",ptr->buffer[position]);		// print the action
			
			ptr->buffer[position] = 0;									// clear the value readed
			
			write(fd_child_parent[1], &aux2, sizeof(char));
			
		}

		close(fd_parent_child[0]);
		close(fd_child_parent[1]);
		
        exit(0);
	}
	
	if (munmap(ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Erro! -  Unmap the shared memory segment");	
		exit(1);
	}
	close(fd);
	if (shm_unlink("/shmtest") < 0) {	// Unlink the shared memory segment (removes the shared memory segment from the file
		perror("Erro! - unlink the shared memory segment"); 
		exit(1);
	}
	
return 0;
}
