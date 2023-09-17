#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int flag = 0;

void handle_SIGUSR1(int signo){
	flag = 1;
}


int main(void) {
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));							// Clear the act variable.
	sigemptyset(&act.sa_mask); 											// No signals blocked 
	act.sa_handler = handle_SIGUSR1;									// Pointer to an ANSI C handler function
	sigaction(SIGUSR1, &act, NULL);										// Set up signal handler for SIGUSR1 signal
	
	
	int status;
	pid_t p = fork();                                                   //Creates Proccess
	
	if(p < 0){                                                          //Verify state of Proccess
        perror("Error creating Fork");                                  //
        return 1;                                                       //
        }
    
	if(p == 0){                                                         //Child Proccess
		srand(time(NULL)); 												//Seed the random number generator with the current time
		int rand_num = (rand() % 5) + 1; 								//Generate a random number between 1 and 5
		sleep(rand_num); 												//Pause the program for the random number of seconds
		printf("Task B: done!\n");										//Print Task B
		while(flag == 0){												//'Infinite' Loop to check if Task A is done
			
			}
		sleep(1);
		printf("Task C: done!\n");										//Print Task C					
			
		
		
		
		
	}else{                                                              //Father Proccess
		printf("Executing Task A\n");
		sleep(3);
		printf("Task A: done!\n");										//Print Task A
		kill(p,SIGUSR1);												//Send Signal to child
		waitpid(p,&status,0);											//Wait child ends
		printf("Job is complete!\n");									//Print that all tasks are completed
	}
	
	
	
} 


