#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

/*In this function, the parent will create 4 children and will wait
only for the children with an even PID.
The children's exit value is its index of creation, which is printed
in the parent process.*/
void main()
{
    int i;
    int status;
    pid_t p; 

    for (i = 0; i < 4; i++) {
        p = fork();

        if (p == -1) {
            perror("Fork falhou"); exit(1);
        }
        
        if (p == 0) {
            printf("Child %d is sleeping...\n", getpid());
            sleep(1);
            exit(i + 1);
            
        }else{
            //check if child has an even PID
            if ((p % 2) == 0)
            {
                printf("parent will wait for %d\n", p);
                waitpid(p, &status, 0);

                if (WIFEXITED(status)){
                    printf("Child %d finished.\n", p);
                    printf("Child %d is the %d°\n", p, WEXITSTATUS(status));
                }
            }else{
                printf("Child %d is the %d°\n", p, WEXITSTATUS(status) + 1);
            }
        }
    }
    
    printf("This is the end.\n");
}