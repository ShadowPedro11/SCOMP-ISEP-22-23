#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void handle_SIGINT(int signo){														//function that will be called when the process receives
	write(STDOUT_FILENO, "I won’t let the process end with CTRL-C!\n", 40);			//the signal SIGINT
}

void handle_SIGQUIT(int signo){														//function that will be called when the process receives
	write(STDOUT_FILENO, "I won’t let the process end by pressing CTRL-\!\n", 55);	//the signal SIGQUIT
}										

int main(void) {
	struct sigaction act;												// 
	memset(&act, 0, sizeof(struct sigaction));							// Clear the act variable.
	sigemptyset(&act.sa_mask); 											// No signals blocked 
	act.sa_handler = handle_SIGINT;										// Pointer to an ANSI C handler function
	sigaction(SIGINT, &act, NULL);										// Set up signal handler for SIGINT signal
	
	struct sigaction act2;												// 
	memset(&act2, 0, sizeof(struct sigaction));							// Clear the act2 variable.
	sigemptyset(&act2.sa_mask); 										// No signals blocked
	act2.sa_handler = handle_SIGQUIT;									// Pointer to an ANSI C handler function
	sigaction(SIGQUIT, &act2, NULL);									// Set up signal handler for SIGQUIT signal
	
	for(;;){															// Infinite loop to print a message every second
		printf("I Like Signal\n");
		sleep(1);
 }

} 


