#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

/*This function will spawn "n" children processes.
To the child process it returns its creation index and to the parent
it returns 0;*/
int spawn_childs(int n){
    pid_t p;
    for (int i = 0; i < n; i++) {
        p = fork();

        if (p == -1) {
            perror("Fork failed!"); exit(1);
        }

        if (p == 0) {
            return i + 1;
        }
    }
    return 0;
}

/*This funtion creates 6 childs. Each child's exit value is its 
creation index times 2. The parent process will wait for all childs
and then print its exit values.*/
int main(){
    int n = 6;
    int status;
    int index = spawn_childs(n);
    
    if (index > 0) {
        exit(index * 2);
    } else {
        while (wait(&status) > 0) {
            if (WIFEXITED(status)) {
                printf("Index: %d\n", WEXITSTATUS(status));
            }
        }
    }

    return 0;
}