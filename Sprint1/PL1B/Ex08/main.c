#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

/*Handler of the signal SIGUSR1. Will print the wanted message.*/
void handler(int signo, siginfo_t *sinfo, void *context){
    char msg [80];
    sprintf(msg, "I captured a SIGUSR1 sent by the process with PID %d\n", sinfo->si_pid);
    write(STDOUT_FILENO, msg, strlen(msg));
}

/*This function enters an infinite while loop and will handle the SIGUSR1 signals
sent by a process.*/
int main(){
    while (1) {
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        sigemptyset(&act.sa_mask);
        act.sa_sigaction = handler;
        act.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &act, NULL);
    }

    return 0;
}