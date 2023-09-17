#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CHILDS_NUM 50

volatile sig_atomic_t nr_childs = 0;
volatile sig_atomic_t relevant = 0;

/*This function generates a random value between 1 and 100. It then checks if the number is
less or equal to 5. If it is, it returns 1. If not, it returns 0.*/ 
int simulate1(){
    srand(time(NULL)+ getpid());

    int num = (rand() % 100) + 1;

    if (num < 6) {
        return 1;
    } else {
        return 0;
    }
}


/*This function's purpose is only to signal that the child process reached this point.*/
void simulate2(){
    char msg[100];
    sprintf(msg, "Child %d reached simulate2()\n", getpid());
    write(STDOUT_FILENO, msg, strlen(msg));
}

/*This is the function that will handle the SIGUSR1 signal received by the parent process.
It increments the variable "nr_childs", meaning that a child has finished the execution of 
"simulate1()". It also increments the variable "relevant" meaning that it found relevant data.*/
void handle_USR1_parent(int signo){
    nr_childs++;
    relevant++;
}

/*This is the function that will handle the SIGUSR2 signal received by the parent process.
It increments the variable "nr_childs", meaning that a child has finished the execution of 
"simulate1()".*/
void handle_USR2_parent(int signo){
    nr_childs++;
}

/*This is the function that will handle the SIGUSR1 signal received by the child process.*/
void handle_USR1_child(int signo){
    simulate2();
}

/*This function will create 50 children in a for loop. The parent will be waiting for the child 
to send a signal, either SIGUSR1 or SIGUSR2, and handle it accordingly. When 25 childs send a signal, 
the parent  will check if it was found relevant data. If it was, the process will send a SIGUSR1 to every 
child, meaning that every child will start to execute "simulate2()". If it wasn't, the parent will kill all
the children.*/
int main(){
    pid_t pid_array[CHILDS_NUM];
    int status_array[CHILDS_NUM];
    
    //Generate childs
	for(int i = 0; i < CHILDS_NUM;i++){
        pid_array[i] = fork();

        if (pid_array[i] == -1){
            printf ("Fork failed in child %d\n", i);
            exit(EXIT_FAILURE);
        }
                      
        if (pid_array[i] != 0){
            //parent 

            struct sigaction act_SIGUSR1;
            memset(&act_SIGUSR1, 0, sizeof(struct sigaction));
            sigfillset(&act_SIGUSR1.sa_mask);
            act_SIGUSR1.sa_handler = handle_USR1_parent;
            sigaction(SIGUSR1, &act_SIGUSR1, NULL);

            struct sigaction act_SIGUSR2;
            memset(&act_SIGUSR2, 0, sizeof(struct sigaction));
            sigfillset(&act_SIGUSR2.sa_mask);
            act_SIGUSR2.sa_handler = handle_USR2_parent;
            sigaction(SIGUSR2, &act_SIGUSR2, NULL);
        
            pause();
            
            if (nr_childs >= 25){	
                //Check if it found relevant data
                if (relevant == 0){
                    printf("Inefficient algorithm!\n");
                    
                    for (int i = 0; i < CHILDS_NUM; i++) {
                        kill(pid_array[i], SIGKILL); 
                    }
                } else {
                    for (int i = 0; i < CHILDS_NUM; i++) {
                        kill(pid_array[i], SIGUSR1);  
                    }
                }
            }

        } else {
            //child
            
            struct sigaction act;
            memset(&act, 0, sizeof(struct sigaction));
            sigfillset(&act.sa_mask);
            act.sa_handler = handle_USR1_child;
            sigaction(SIGUSR1, &act, NULL);

            if (simulate1() == 0){
                kill(getppid(), SIGUSR2);
            } else {
                kill(getppid(), SIGUSR1);
            }
            
            pause();
            exit(0);
        }
    }
    return 0;
}
