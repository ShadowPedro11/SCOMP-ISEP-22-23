#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

/*This function creates two processes. The parent reads the content of
a file and sends it to the child, one string at a time. After that it waits
for the child to finish executing.
The child reads the content of the file from the pipe and prints it.
*/
int main() {
    int fd[2];
    int status;

    if (pipe(fd) == -1) {
        perror("Pipe failed!\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1){
        perror("Fork failed!\n");
        return 1;
    }

    if (pid > 0) {
        close(fd[0]);   //Close read

        //Open file
        FILE *ptr_file = fopen("file.txt", "r");

        char buff[100];

        if (ptr_file != NULL){
            while (fgets(buff, 100, ptr_file) != NULL){
                if (write(fd[1], buff, strlen(buff)) == -1){
                    perror("Write failed!\n");
                    exit(EXIT_FAILURE);
                }
            } 
        }

        fclose(ptr_file);   //Close file

        close(fd[1]);   //Close write

        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status)){
            printf("Child finished!\n");
        }

    } else {
        close(fd[1]);   //Close write
        char buff[100];

        if (read(fd[0], buff, 100) == -1){
            perror("Read failed!\n");
            exit(EXIT_FAILURE);
        }
        printf("%s", buff);

        printf("\n");
        
        close(fd[0]);   //Close read
        exit(EXIT_SUCCESS);
    }

    return 0;
}