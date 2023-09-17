#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#define ARRAY_SIZE 1000

int main(){
	int vec1[ARRAY_SIZE];	                 							// array to lookup 
	int vec2[ARRAY_SIZE];	                 							// array to lookup 
	int result[ARRAY_SIZE];	                 							// array to lookup 
	time_t t;															// needed to initialize random number generator (RNG) 
	int i;

	srand ((unsigned) time (&t));										// intializes RNG (srand():stdlib.h; time(): time.h) 

	for (i = 0; i < ARRAY_SIZE; i++){									// initialize arrays with random numbers (rand(): stdlib.h) 
		vec1[i] = rand () % 255;
		vec2[i] = rand () % 255;
	}
     
    int size = 5;														//size -> number of childs
    
    int fd[5][2];														//Pipe

	for (int i = 0; i < size; i++) {									// Creates 5 pipes 
		if (pipe(fd[i]) < 0) {
			perror("Pipe failed");
			exit(1);
		}
	}

	pid_t pid_array[size];												//creation of an array of pid_t to create all child's
	int status_array[size];

	for(i = 0; i < size;i++){											//fori cycle to create all childs

		pid_array[i] = fork();											//create cycle at position i of the pid_array

		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");
			return 1;
		}

		if(pid_array[i] == 0){											//In the child process
			
			for(int j = 0; j < size; j++) {								//For cycle close pipes
				close(fd[j][0]);
				if(j != i)
					close(fd[j][1]);
			}					

			int num;													//variable to write values
			int initial = i*200;										//initial value to search on child
			int end = initial+200;										//end value to search on child
		
			for(int j = initial; j < end; j++){							//Run all values between initial and end position on arrays
				num = vec1[j] + vec2[j];								//add the sum to num
				write(fd[i][1], &num, sizeof(int));						//writes in pipe 
			}
			
			close(fd[i][1]);											//close pipe
	
			exit(0);
		}
	}

	for(i = 0; i < size; i++){											//Wait the end of all child process
		waitpid(pid_array[i], &status_array[i], 0);						//
	}

	for(i = 0; i < size; i++){											//Cycle to read the values obtained

		close(fd[i][1]);
		
		int num;														//variable to read the values
		int initial = i*200;											//initial of cycle
		int end = initial+200;											//end of cycle
		
		for(int j = initial; j < end; j++){								//run at initial to end
			read(fd[i][0], &num, sizeof(int));							//read values
			result[j] = num;											//put the value on an array called results
		}
		
		close(fd[i][0]);												//close the pipe
	}
	
	for (int i = 0; i < ARRAY_SIZE; i++) {								//Print all values
        printf("%d : %d + %d -> %d\n", i+1,vec1[i] ,vec2[i],result[i]);
    }
 

	
	
	return 0;
    }
