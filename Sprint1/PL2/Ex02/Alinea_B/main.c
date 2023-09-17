#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

/*This function opens the pipe, checking if it failed. Then creates two
processes. 
The parent reads an integer and a string, stores them in a struct of type
var and sents the struct through the pipe in one write operations.
The child reads from the pipe the struct with the integer and the string 
and then prints them.*/
int main() {
    int fd[2];

    typedef struct {
        char string[20];
        int num;
    } var;

    if (pipe(fd) == -1) {
        perror("Pipe failed!\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1){
        printf("Fork failed!\n");
        return 1;
    }

    if (pid > 0) {
        //parent 

        close(fd[0]);   //Close read

        var v1; 

        printf("Insert an integer\n");
        scanf("%d", &v1.num);
        printf("Insert a string\n");
        scanf("%s", v1.string);

        if (write(fd[1], &v1, sizeof(var)) == -1) {
            perror("Write failed!\n");
            exit(EXIT_FAILURE);
        }

        close(fd[1]);
    } else {
        //child
        
        close(fd[1]);   //Close write

        var v2;

        if (read(fd[0], &v2, sizeof(var)) == -1){
            perror("Read failed!\n");
            exit(EXIT_FAILURE);
        }

        printf("\nChild read:\nnum = %d\nstring = %s\n", v2.num, v2.string);

        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    return 0;
}