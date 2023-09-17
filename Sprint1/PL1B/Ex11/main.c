#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void handle_SIGUSR1(int signo){											//Function that will be called when the process receives the signal SIGUSR1
	sigset_t mask;														//
	sigprocmask(SIG_BLOCK, NULL, &mask);								//Blocks all signals in the signal set specified by the mask.					
	
	write(STDOUT_FILENO, "Sinais bloquados:\n", 18);
	
	int i;
    for (i = 1; i <= 64; i++) { 										//Run all Signals 
        if (sigismember(&mask, i)) {									//Checks if signal i is blocked.
            char sig[4];
            sprintf(sig, "%d;", i);										// Converts the signal number to a string.
            write(STDOUT_FILENO, sig, strlen(sig));						// Prints the signal number to the console.
        }
        }
    }
   
    write(STDOUT_FILENO, "\n", 1);
	
	
}

int main(void) {
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));							// Clear the act variable.
	sigfillset(&act.sa_mask);											// Sets the signal mask of act to include all signals.
	
	act.sa_handler = handle_SIGUSR1;									// Pointer to an ANSI C handler function
	sigaction(SIGUSR1, &act, NULL);										// Set up signal handler for SIGUSR1 signal
	
	
	for(;;){															// Infinite loop to print a message every second
		printf("I Like Signal\n");
		sleep(1);
	}	
	
	
} 


