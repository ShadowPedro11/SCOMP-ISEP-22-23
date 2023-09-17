#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(void) {
	int pid, signal_num;
	
		printf("Enter the target PID: ");								//Enter the pid of the process
		scanf("%d", &pid);

		printf("Enter the signal number to send: ");					//Enter Signal to send
		scanf("%d", &signal_num);

		if (kill(pid, signal_num) == -1) {								//Sending Signal
			perror("kill error");
			exit(0);
		}

		printf("Signal %d sent to process %d.\n", signal_num, pid);		//
		
    

	
} 


