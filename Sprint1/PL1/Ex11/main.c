#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#define ARRAY_SIZE 1000

int main(){
	int numbers[ARRAY_SIZE];	                 						/* array to lookup */
	time_t t;															/* needed to initialize random number generator (RNG) */
	int i;
  
	srand ((unsigned) time (&t));										/* intializes RNG (srand():stdlib.h; time(): time.h) */
  
	for (i = 0; i < ARRAY_SIZE; i++){									/* initialize array with random numbers (rand(): stdlib.h) */
	  numbers[i] = rand () % 255;
	}
    
	
	int size = 5;														//size -> number of childs
	pid_t pid_array[size];												//creation of an array of pid_t to create all child's
	int max=0;
	int status_array[size];
	
	for(i = 0; i < size;i++){											//fori cycle to create all childs
		
	pid_array[i] = fork();												//create cycle at position i of the pid_array
	
	if(pid_array[i] < 0){												//Check if the process was created
		perror("Error creating Fork");                                  //			
		return 1;                                                       //
	}
	
	if(pid_array[i] == 0){												//In the child process
		int initial = i*200+1;
		int end = initial+199;
		for(int j = initial; j < end; j++){
			if(numbers[j] > max){			
				max = numbers[j];
				}
			}
			printf("Child %d PID [%d] max value = %d\n",i+1, getpid(), max);
			exit(max);
		}
	}
	
	for(i = 0; i < size; i++){
		waitpid(pid_array[i], &status_array[i], 0);						//
	}
	
	for(i = 0; i < size; i++){
		if(WIFEXITED(status_array[i])){									//
			if(WEXITSTATUS(status_array[i]) > max){						//
				max = WEXITSTATUS(status_array[i]);
			}
		}
	}
	printf("\nMaximum value in the array is %d\n\n", max);
	
	int result[size];
	pid_t pid = fork();
	
	if(pid < 0){														//Check if the process was created
		perror("Error creating Fork");                                  //			
		return 1;                                                       //
	}
	
	if(pid == 0){	
		
		for(i = 0; i < (ARRAY_SIZE>>1); i++){
			result[i]=((int) numbers[i]/max)*100;
			printf("%d, %d\n",i+1,result[i]);
		}
	
	}else{
		wait(NULL);
		for(i = (ARRAY_SIZE>>1); i < ARRAY_SIZE; i++){
			result[i]=((int) numbers[i]/max)*100;
			printf("%d, %d\n",i+1,result[i]);
		}

	}
	
    return 0;
}
