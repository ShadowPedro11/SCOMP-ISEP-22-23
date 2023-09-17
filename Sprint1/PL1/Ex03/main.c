#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
	fork(); fork(); fork();
	printf("SCOMP!\n");
	return 0;
}
