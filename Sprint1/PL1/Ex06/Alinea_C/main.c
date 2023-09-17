#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

/*In this function, the parent will create 4 children and will wait
only for the children with an even PID.*/
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
            printf("child %d is sleeping\n", getpid());
            sleep(1);
            exit(0);
            
        }else{
            //check if child has an even PID
            if ((p % 2) == 0)
            {
                printf("parent will wait for %d\n", p);
                waitpid(p, &status, 0);

                if (WIFEXITED(status)){
                    printf("--Child %d finished.\n", p);
                }
            }
        }
    }
    printf("This is the end.\n");
}