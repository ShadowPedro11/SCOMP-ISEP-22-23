#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <semaphore.h>


int main () {
    sem_t *sem_w = sem_open("/sem_w",O_CREAT|O_EXCL, 0644, 1);
	if(sem_w == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}    

    sem_t *sem_diff = sem_open("/sem_diff",O_CREAT|O_EXCL, 0644, 2);
	if(sem_diff == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	} 

    pid_t pid = fork();

    if (pid == -1){
        printf ("Fork failed!\n");
        exit(EXIT_FAILURE);
    }

    if (pid > 0){
        //father
    
        while (1) {
            sem_wait(sem_diff);
                sem_wait(sem_w);
                    printf("S");
                    fflush(stdout); 
                sem_post(sem_w);
            sem_post(sem_diff);

            struct timespec sleep;
            sleep.tv_sec = 0;
            sleep.tv_nsec = rand() % 200000000;
            nanosleep(&sleep, NULL); 
        }

    } else {
        //child
        while (1) {
            sem_wait(sem_diff);
                sem_wait(sem_w);
                    printf("C");
                    fflush(stdout); 
                sem_post(sem_w);
            sem_post(sem_diff);
            
            struct timespec sleep;
            sleep.tv_sec = 0;
            sleep.tv_nsec = rand() % 200000000;
            nanosleep(&sleep, NULL); 
        }

        exit(EXIT_SUCCESS);
    }

    wait(NULL);
    printf("\n");

    sem_unlink("/sem_w");
    sem_unlink("/sem_diff");

    return 0;
}