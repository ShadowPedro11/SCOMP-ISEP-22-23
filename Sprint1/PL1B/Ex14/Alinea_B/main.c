#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

void handler(int signo){
    write(STDOUT_FILENO, "You were too slow and this program will end!\n", 45);
    exit(0);
}

/*Function will create two processes. The parent process will ask for a string 
input and will wait for the user to insert it. Meanwhile, the child process
will be sleeping for 10 seconds and then sends a signal to the parent. If the parent 
is still waiting for the user input, the signal will be handled, writing the wanted
phrase and exiting the process. If not, it means that the user gave the string 
within the time limit so the parent process already finished.*/
int main(){
    struct sigaction act;
    pid_t pid = fork();

    if (pid > 0){
        memset(&act, 0, sizeof(struct sigaction));
        sigemptyset(&act.sa_mask);
        act.sa_handler = handler;
        sigaction(SIGUSR1, &act, NULL);

        char string[80];
        write(STDOUT_FILENO, "Enter a string:\n", 16);
        scanf("%[^\n]", string);

        char print[40];
        sprintf(print, "The string's length is: %ld\n", strlen(string));
        write(STDOUT_FILENO, print, strlen(print));
    } else {
        sleep(10);
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    return 0;
}