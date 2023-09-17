#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define SIZE_ARRAY 5


typedef struct {
 char cmd [32]; 	// Command name .
 int time_cap ; 	// Time limit to complete (in seconds ).
 }command_t ;

int check = 0;

void handle_SIGCHLD(int signo){											//handle SIGCHLD
	check = 1;  														// When a signal SIGCHLD is sended check is 1
}


int main(void) {
	
	int i;
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));							// Clear the act variable.
	sigemptyset(&act.sa_mask); 											// No signals blocked 
	act.sa_handler = handle_SIGCHLD;									// Pointer to an ANSI C handler function
	sigaction(SIGCHLD, &act, NULL);										// Set up signal handler for SIGCHLD signal
	
	command_t array[] = {{"./a", 3}, {"./b", 5}, {"./c", 10}, {"./d", 15}, {"./e", 20}};  //Set up array of commands
	
	for(i=0;i<sizeof(array)/sizeof(command_t);i++){ 					//Run array of command's 
		pid_t p = fork();												//Create child process 
		if(p < 0){														//Chech creation of child process 
			perror("Error creating process!");
			return 1;
		}
		if(p == 0){														//Execution of child
            execlp(array[i].cmd, array[i].cmd, (char*)NULL);			//Sub execution of child to the programm on command
            exit(0);													//exit
		}else{	
			int tempo=0;												//Initialize variable tempo to check time execution
			while(check == 0 && tempo<array[i].time_cap){				//While cycle that have a sleep of 1 seconde and an increment on tempo
				sleep(1);												//This cycle will end when tempo is bigger than the time limit or
				if(check==0){											//check is diff of 0 so the child process is terminated
					tempo++;
				}
			}
			if(check == 0){					//If tempo >= array[i].time_cap te program exceced limit time
				kill(p, SIGKILL);										//kill the process
                printf("The command %s didn't end in its allowed time!\n", array[i].cmd);
			}
        wait(NULL);														//wait to child end's
        check = 0;														//reset of variable check
			}
	}
} 


