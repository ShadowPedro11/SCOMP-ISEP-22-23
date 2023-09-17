#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>

volatile sig_atomic_t USR1_counter = 0;

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
    /*When we change the handler to take more than one second it seems like the process only
    handles the first SIGUSR1 to arrive and then jumps to the SIGINT, stopping the process.*/
    sleep(3);
    char msg[80];
    USR1_counter++;
    sprintf (msg, "SIGUSR1 signal captured: USR1_counter = %d\n", USR1_counter);
    write(STDOUT_FILENO, msg, strlen (msg));
}

int main(){
    pid_t pid = fork();

    if (pid == 0){
        struct timespec sleep_time, remaining_time;
        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = 10;

        /*Child process will send 12 SIGUSR1s to parent*/
        for (int i = 0; i < 12; i++) {
            kill(getppid(), SIGUSR1);
            if (nanosleep(&sleep_time, &remaining_time)== -1){
                printf ( "nanosleep was interrupted!\n");
            }
        }

        kill(getppid(), SIGINT);
        exit(0);
            
    } else {
        /*Parent process will be in an infinite loop*/
        while (1) {
            printf("I'm working!\n");
            
            struct sigaction act;
            memset(&act, 0, sizeof(struct sigaction));
            /*No signals are being blocked (point (e)) which makes
            the parent process incapable of handling all the SIGUSR1 
            signals, unlike in point (d), since the handling function is much
            slower than the speed the signals are being sent from the child.*/
            sigemptyset(&act.sa_mask);
            act.sa_sigaction = handle_USR1;
            act.sa_flags = SA_SIGINFO;
            sigaction(SIGUSR1, &act, NULL);
            sleep(1); 
        }
    }
    return 0;
}