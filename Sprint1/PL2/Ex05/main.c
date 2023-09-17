#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 80

void toUpper(char *string) {											// FUnction to transfrom in Upper Case
    for(int i = 0; i < strlen(string); i++) {
        if(string[i] >= 'a' && string[i] <= 'z') { 						// if it's lower case
            string[i] = string[i] - 32; 								// convert to uppercase using ASCII table
        }
    }
}

int main(void) {
	char read_msg[BUFFER_SIZE];
	char write_msg[BUFFER_SIZE];
	
	int up[2];
	int down[2];
	
	if(pipe(up) == -1){													/* Creates pipe */
		perror("Pipe failed");
		return 1;
	}
	if(pipe(down) == -1){												/* Creates pipe */
		perror("Pipe failed");
		return 1;
	}




	pid_t p = fork();
	if(p < 0){															//Check creation of child process 
		perror("Error creating process!");
		return 1;
		}
		
	if(p == 0){															//Child
		close(down[1]); 												//closes unused write descriptor 
		close(up[0]); 													//closes unused read descriptor 
		
		printf("Enter a message: ");									//
        fgets(write_msg, BUFFER_SIZE, stdin);							//Read message from console
		
		write(up[1], write_msg, BUFFER_SIZE);							//writes in pipe 
		
        read(down[0], read_msg, BUFFER_SIZE);							//read from pipe down
        printf("Received message: %s", read_msg);						//print message now in Upper Case
		
		close(down[0]); 												//closing read descriptor of down pipe 
		close(up[1]); 													//closing write descriptor of up pipe 
		exit(0);
	}else{																//Parent
		close(down[0]); 												//closes unused read descriptor 
		close(up[1]); 													//closes unused write descriptor  
		
		read(up[0], read_msg, BUFFER_SIZE); 							//read from pipe pipe 
		toUpper(read_msg);												//function to transform in Upper case
		write(down[1],read_msg,BUFFER_SIZE); 							//writes in pipe 
		
		close(down[1]); 												//closing write descriptor of down pipe 
		close(up[0]); 													//closing read descriptor of up pipe 
		wait(NULL);
	}	
	
} 


