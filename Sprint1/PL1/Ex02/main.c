#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/*This function creates several child processes. The parent creates a child and will wait
for it. The child 1 then creates another child and waits for it. The child 2*/
int main(){
    int status;
    pid_t pid;

    pid = fork();

    

    if (pid == -1){
        perror("Fork falhou"); exit(1);
    }

    if (pid > 0){
        printf("I'm..\n");
    
    }else{
        printf("I'll never join you!\n");
        exit(EXIT_SUCCESS);
    }

    pid = fork();

    if (pid > 0){
        printf("the..\n");
    
    }else{
        printf("I'll never join you!\n");
        exit(EXIT_SUCCESS);
    }

    pid = fork();

    if (pid > 0){
        printf("father!..\n");
    
    }else{
        printf("I'll never join you!\n");
        exit(EXIT_SUCCESS);
    }
    return 0;
}