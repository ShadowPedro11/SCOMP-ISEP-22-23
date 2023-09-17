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

#define CHILDS_NUM 2

/*Spawns a CHILD_NUM number of child processes and stores the pids 
in an array. Child process return its index and exits the function.
Parent process continues the for cicle and at the end returns -1.*/
int spawn_childs(pid_t *pid_array) {
    for (int i = 0; i < CHILDS_NUM; i++){
        pid_array[i] = fork();

        if (pid_array[i] == -1){
            printf ("Fork failed in child %d\n", i);
            exit(EXIT_FAILURE);
        }
        if (pid_array[i] == 0) return i;  
    }
    return -1;
}

int main() {
    pid_t pid_array[CHILDS_NUM];
    int status_array[CHILDS_NUM];

    sem_t *sem_child1 = sem_open("/sem_child1",O_CREAT|O_EXCL, 0644, 1);
	if(sem_child1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}

    sem_t *sem_child2 = sem_open("/sem_child2",O_CREAT|O_EXCL, 0644, 0);
	if(sem_child2 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}    

    sem_t *sem_parent = sem_open("/sem_parent",O_CREAT|O_EXCL, 0644, 0);
	if(sem_parent == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	} 

    int index = spawn_childs(pid_array);        //Create the 2 childs

    for (int i = 0; i < 15; i++) {
        if (index == -1) {
            //parent 
            sem_wait(sem_parent);
            printf("Father\n");
            sem_post(sem_child2);

        } else {
            if (index == 0) {
                //First child
                sem_wait(sem_child1);
                printf("%dst child\n", index + 1);
                sem_post(sem_parent);

            } else {
                //Second child
                sem_wait(sem_child2);
                printf("%dnd child\n", index + 1);
                sem_post(sem_child1);
            }
        }
    }
    
    if (index > -1) {
        exit(EXIT_SUCCESS);
    }
    

    for (int i = 0; i < CHILDS_NUM; i++) {              //Wait for all childs
        waitpid(pid_array[i], &status_array[i], 0);
    }
    
    sem_unlink("/sem_child2");
    sem_unlink("/sem_parent");

    return 0;
}