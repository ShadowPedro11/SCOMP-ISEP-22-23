#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>

volatile sig_atomic_t nr_children_exec = 5;

/*Function to spawn 5 childs. If its the child process it returns
the index of its creation and exits the function. If its the parent, 
it finishes creating the child processes and then returns -1.

The return value is used to differentiate the child processes from 
the parent.*/
int spawn_childs(pid_t *pid_array) {
    for (int i = 0; i < 5; i++){
        pid_array[i] = fork();

        if (pid_array[i] == -1){
            printf ("Fork failed in child %d\n", i);
            
            exit(EXIT_FAILURE);
        }
        if (pid_array[i] == 0) return i;  
    }

    return -1;
}

/*Handler function for when the SIGUSR1 signal is received in the parent 
process. 

It decrements the global variable that is initialize with 5 each time that
the parent process receives the SIGUSR1 signal.*/
void handle_USR1(int signo){
    nr_children_exec--;
}

int main(){
    pid_t pid_array[5];
    int index = spawn_childs(pid_array); 
    struct sigaction act;
    int status;

    if (index == -1){ 
        /*While there are children executing, the parent process will be
        waiting for the SIGUSR1 signal meaning that a child has stopped 
        executing.*/
        while (nr_children_exec != 0){
            memset(&act, 0, sizeof(struct sigaction));
            sigfillset(&act.sa_mask);
            act.sa_handler = handle_USR1;
            sigaction(SIGUSR1, &act, NULL);
            pause();
        }
        
        /*This while loop waits for all children to stop executing to make 
        sure that every child as stopped its execution successfully.*/
        while (wait(&status) > 0) {
            if (WIFEXITED(status)>0) {
                printf("Child %d finished\n", WEXITSTATUS(status));
            }
        }
        
    } else {
        /*For cicle to print the intended integers.*/
        for (int i = (index * 200); i < ((index + 5) * 200); i++){
            printf("i%d = %d\n",index, i);
        }

        kill(getppid(), SIGUSR1);  
        exit(index);   
    }

    return 0;
}