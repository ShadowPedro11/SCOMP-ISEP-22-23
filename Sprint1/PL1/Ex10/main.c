#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>
#define ARRAY_SIZE 2000
#define CHILDREN_NUM 10

/*This function initializes an array with random numbers between 0 and 199*/
void fill_array(int array[], int length){
    srand(time(NULL));
    for (int i = 0; i < length; i++) {
        array[i] = (rand() % 200);
    }
}

/*This function, starts by asking for a number (between 0 and 199) to be found in the 
array. Then it fills the array with 2000 random numbers using the function "fill_array()".
Then it creates 10 childs. Each child will search the array from index (i*200) until 
(200+i*200), "i" being the index of creation of the child. That means that each child will
be responsible for a 1/10 of the array. The child's objective is to find the first occurrence of the
inputed number. If it finds one,  it immediatly exits the process with the exit value being the 
relative index where it was found within the section of the array it computed. If it doesn't find
it exits with the value 255. The parent will be waiting for all the children to finish execution and
storing its exit values in the array "status_array". Finally, if the exit value of the child
was a valid one (in the range [0,200[), it will be printed the valid index by the parent.*/
int main(){
    int array[ARRAY_SIZE];
    pid_t pid_array[CHILDREN_NUM];
    int status_array[CHILDREN_NUM];
    int n;
    int i;

    printf("Insert the number to be found (between 1 and 200):\n");
    scanf("%d", &n);

    fill_array(array, ARRAY_SIZE);

    for (i = 0; i < 10; i++) {
        pid_array[i] = fork();

        if (pid_array[i] == -1) {
            perror("Fork failed.\n"); exit(1);
        }

        //Look for the inputed number
        if (pid_array[i] == 0) {
            for (int j = 0; j < 200; j++) {
                if (array[j + (i*200)] == n) {
                    exit(j);
                }
            }
            exit(255);
        }   
    }
    
    //Wait for all children
    for (i = 0; i < CHILDREN_NUM; i++) {
        waitpid(pid_array[i], &status_array[i], 0);
    }

    //Print the valid indexes
    for (i = 0; i < CHILDREN_NUM; i++){
        if (WIFEXITED(status_array[i])) {
            int pos;
            if (((pos = WEXITSTATUS(status_array[i])) >= 0) && ((pos = WEXITSTATUS(status_array[i])) < 200)){
                printf("Index found: %d\n", (pos + (i*200)));
            }
        }
    }

    return 0;
}

