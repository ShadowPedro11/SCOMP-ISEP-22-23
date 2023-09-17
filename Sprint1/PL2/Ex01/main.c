#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 80

int main(void) {
	char read_msg[BUFFER_SIZE];
	char write_msg[BUFFER_SIZE];
	sprintf(write_msg,"%d",getpid());
	
	int fd[2];
	if(pipe(fd) == -1){													/* Creates pipe */
		perror("Pipe failed");
		return 1;
	}

	pid_t p = fork();
	if(p < 0){															//Check creation of child process 
		perror("Error creating process!");
		return 1;
		}
		
	if(p == 0){															//Child
		close(fd[1]); 													// closes unused write descriptor 
		read(fd[0], read_msg, BUFFER_SIZE); 							// read from pipe pipe 
		printf("Parent Process is: %s\n", read_msg);
		close(fd[0]);													// closes read descriptor 
		
	}else{																//Parent
		printf("My id is:%d\n",getpid());								
		close(fd[0]); 													// closes unused read descriptor 
		write(fd[1],write_msg,strlen(write_msg)+1); 					// writes in pipe 
		close(fd[1]); 													// closing write descriptor 
	}	
	
	
} 


