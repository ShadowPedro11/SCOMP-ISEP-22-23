#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
	int begin = 1;														//first element to print
	int end = 100;														//last element to print
	int size = 10;														//size -> number of childs
	pid_t pid_array[size];												//creation of an array of pid_t to create all child's
	
	for(int i = 0; i < size;i++){										//fori cycle to create all childs
		
	pid_array[i] = fork();												//create cycle at position i of the pid_array
	
	if(pid_array[i] < 0){												//Check if the process was created
		perror("Error creating Fork");                                  //			
		return 1;                                                       //
	}
	
	if(pid_array[i] == 0){												//In the child process
		printf("%d. ",i+1);
		for(int j = begin; j<=end;++j){									//fori cycle to print the numbers				
		printf("%d-",j);
		}
		printf("\n\n");
		exit(0);														//exit the process
		}
		
		begin+=100;														//add 100 to begin
		end+=100;														//add 100 to end
	}
	
	for(int h = 0; h < size; h++){
		waitpid(pid_array[h], NULL, 0);									//waits childs to finish
	}

    return 0;
}
