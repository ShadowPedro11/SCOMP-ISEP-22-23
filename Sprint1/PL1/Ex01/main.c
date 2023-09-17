#include <stdio.h>
#include <sys/types.h> //pid_t
#include <unistd.h>	   //fork()

int main(void) {
	int x = 1;
	pid_t p = fork();                      /*pid_t: sys/types.h; fork(): unistd.h*/
	if (p == 0) {                         // p = 0 -> Processo filho
		x = x+1;                          //
		printf("1. x = %d\n", x);         //
	}else{                                // p > 0 -> Processo pai
		x = x-1;
		printf("2. x = %d\n", x);
		}
		printf("3. %d; x = %d\n", p, x);
	
} 

// p = -1 -> Processo falhou
