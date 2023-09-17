#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define VEC_SIZE 1000

/*Spawns 5 child processes and stores the pids in an array.
Child process return its index and exits the function.
Parent process continues the for cicle and at the end returns -1.*/
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

/*Fills an array with a certain length with random numbers between 1 
and 200.*/
void fill_array(int *array, int length){
    //srand(time(NULL));
    for (int i = 0; i < length; i++) {
        array[i] = (rand() % 200) + 1;
    }
}


/*This function will initialize two arrays and fill each with 1000 integers
using the function fill_array(). Then opens the pipe and spawns 5 childs. Each child
will be in charge of summing the elements of the array between the indexes (index*200)
and 199 + (index*200), 'index' being the index of the child. When the child finishes the sum, 
it sends the result through the pipe to the parent. The parent will read all five partial 
sums and calculate the final result after it reads all the child's results.*/
int main(){
    int fd[2];
    int vec1[VEC_SIZE];
    int vec2[VEC_SIZE];
    int status;

    fill_array(vec1, VEC_SIZE);
    fill_array(vec2, VEC_SIZE);

    if (pipe(fd) == -1) {
        perror("Pipe failed!\n");
        return 1;
    }

    pid_t pid_array[5];

    int index = spawn_childs(pid_array);

    //if it is the parent process (index == -1)
    if (index == -1){
        close(fd[1]);   //Close write

        int res = 0;
        int tmp;
        
        //Reads the 5 partial sums and sums them 
        for (int i = 0; i < 5; i++) {
            if (read(fd[0], &tmp, sizeof(int)) == -1) {
                perror("Read failed!\n");
                exit(EXIT_FAILURE);
            }
            res += tmp;
        }
        printf("Result: %d\n", res);

        for (int i = 0; i < 5; i++) {
            waitpid(pid_array[i], &status, 0);
        }
        
        close(fd[0]);   //Close read

    } else {
        close(fd[0]);   //Close read
        int tmp = 0;
        for (int j = 0; j < 200; j++) {
            tmp += vec1[j + (index*200)] + vec2[j + (index*200)];
        }

        if (write(fd[1], &tmp, sizeof(int)) == -1){
            printf("Write failed on child %d!\n", getpid());
            exit(EXIT_FAILURE);
        }

        close(fd[1]);   //Close write
        exit(EXIT_SUCCESS);
    }
    return 0;
}