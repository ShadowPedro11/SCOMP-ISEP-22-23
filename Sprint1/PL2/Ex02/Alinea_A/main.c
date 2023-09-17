#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

/*This function opens the pipe, checking if it failed. Then creates two
processes. 
The parent reads an integer and a string from command line and sents 
them through the pipe in two separate write operations.
The child reads from the pipe the integer and the string and then prints them.*/
int main() {
    int fd[2];

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
        int num;
        char string[50];

        printf("Insert an integer\n");
        scanf("%d", &num);
        printf("Insert a string\n");
        scanf("%s", string);

        if (write(fd[1], &num, sizeof(int)) == -1) {
            perror("Write failed!\n");
            exit(EXIT_FAILURE);
        }
        
        if (write(fd[1], string, strlen(string)) == -1){
            perror("Write failed!\n");
            exit(EXIT_FAILURE);
        }

        close(fd[1]);
    } else {
        //child
        
        close(fd[1]);   //Close write

        int num;
        char string[50];

        if (read(fd[0], &num, sizeof(int)) == -1){
            perror("Read failed!\n");
            exit(EXIT_FAILURE);
        }

        if (read(fd[0], string, 50) == -1){
            perror("Read failed!\n");
            exit(EXIT_FAILURE);
        }

        printf("\nChild read:\nnum = %d\nstring = %s\n", num, string);

        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    return 0;
}