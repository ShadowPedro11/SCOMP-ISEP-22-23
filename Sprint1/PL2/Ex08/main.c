#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CHILDS_NUM 10

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

/*Struct to store the data to be writen and read from the pipe*/
typedef struct {
    int round_number;
    char msg[4];
} game;

/*This function creates 10 child processes using the function spawn_childs(). Then the parent,
using a for cycle, fills the strcut "g_round_info" each 2 seconds with the message "Win" and the 
round's number and writes this in the pipe. The children will try to read this data and the first
to read it will print the message and the round's number and then end its execution with an exit 
value equal to the winning round’s number. The parent will be waiting for any child to stop executing
and when one does, it stores the PID of the child as well as the exit value in the array "results_info".
This repeats 10 times (which is the number of children) and when all the children terminate, the parent 
will print the PID and the winning round of each child.*/
int main(){
    int fd[2];
    pid_t pid_array[10];

    if (pipe(fd) == -1){
        perror("Pipe failed!\n");
        return 1;
    }

    int index = spawn_childs(pid_array);

    if (index == -1) {
        //parent

        close(fd[0]);   //Close read
        game g_round_info;
        int status;
        int results_info[10][2];

        for (int i = 0; i< CHILDS_NUM; i++){ 
            sprintf(g_round_info.msg, "Win");
            g_round_info.round_number = i + 1;

            if (write(fd[1], &g_round_info, sizeof(game)) == -1){
                printf("Write failed!\n");
                exit(EXIT_FAILURE);
            }

            pid_t child_pid = wait(&status);
            if (WIFEXITED(status)){
                results_info[i][0] = child_pid;
                results_info[i][1] = WEXITSTATUS(status);
            }
            sleep(2);
        }
        close(fd[1]);   //Close write

        for (int i = 0; i < CHILDS_NUM; i++){
            printf("Child %d won round %d\n", results_info[i][0], results_info[i][1]);
        }
        
    } else {
        //child 

        close(fd[1]);   //Close write
        game g_round_info;

        if (read(fd[0], &g_round_info, sizeof(game)) == -1) {
            perror("Read failed!\n");
            exit(EXIT_FAILURE);
        }
        
        printf("%s round %d\n", g_round_info.msg, g_round_info.round_number);
        close(fd[0]);   //Close read
        exit(g_round_info.round_number);
    }
    return 0;
}