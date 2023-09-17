#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 80

int main(void) {
	char read_msg1[BUFFER_SIZE];
	char read_msg2[BUFFER_SIZE];
	char write_msg1[BUFFER_SIZE]="Hello World\n";
	char write_msg2[BUFFER_SIZE]="Goodbye!\n";
	
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
		printf("CHILD - PID:%d\n",getpid());
		close(fd[1]); 													// closes unused write descriptor 
		read(fd[0], read_msg1, strlen(write_msg1)+1); 					// read from pipe pipe 
		read(fd[0], read_msg2, strlen(write_msg2)+1); 					// read from pipe pipe 
		printf("MSG1: %s\n",read_msg1);
		printf("MSG2: %s\n",read_msg2);
		close(fd[0]);													// closes read descriptor 
		exit(12);
	}else{																//Parent
		close(fd[0]); 													// closes unused read descriptor 
		write(fd[1],write_msg1,strlen(write_msg1)+1); 					// writes in pipe 
		write(fd[1],write_msg2,strlen(write_msg2)+1); 					// writes in pipe 
		close(fd[1]); 													// closing write descriptor 
	}	
	int status;
	waitpid(p,&status,0);
	printf("------------------------------------\n");
	printf("PARENT:\nCHILD ID -> %d\nCHILD EXIT VALUES -> %d\n",p,WEXITSTATUS(status));	
	printf("------------------------------------\n");
} 


