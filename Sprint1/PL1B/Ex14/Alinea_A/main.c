#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

void handle_ALARM(int signo){
    write(STDOUT_FILENO, "You were too slow and this program will end!\n", 45);
}

/*The program will ask for the user to input a string. If the user
does it under 10 seconds, the program will compute the string's length 
through the function strlen() and display it. If not, the handle_ALARM()
funtion is triggered and display the intended phrase as well as the 
length of the string as 0.*/
int main(){
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_handler = handle_ALARM;
    sigaction(SIGALRM, &act, NULL);

    /*Set alarm for 10 seconds.*/
    alarm(10);

    /*Where the input string will be stored.*/
    char string[100];

    write(STDOUT_FILENO, "Enter a string:\n", 16);
    scanf("%[^\n]", string);

    char print[40];
    sprintf(print, "The string's length is: %ld\n", strlen(string));
    write(STDOUT_FILENO, print, strlen(print));


    return 0;
}