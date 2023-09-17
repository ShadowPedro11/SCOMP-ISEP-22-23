#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>

#define STR_SIZE 50
#define NR_DISC 10
typedef struct{
	int numero;
	char nome[STR_SIZE];
	int disciplinas[NR_DISC];
	int flag;
}aluno;



int main(int argc, char *argv[]){
	

	
	//shm_unlink("/shmtest");												// Unlink (delete) any existing shared memory segment with the same name
	
	int fd, data_size, trunk;
	aluno *ptr;
	
	data_size = sizeof(aluno);
	
	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it to the size of nums struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);												// Truncate the shared memory segment to the size of nums 
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (aluno *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}

	
    
    ptr->flag = 0;														// Assign the value of flag to 0
	pid_t pid = fork();													// Create new proccess
	
	if(pid < 0){														//Check creation of child process 
		perror("Error creating process!");
		return 1;
		}
	
	if(pid > 0){
		pid_t pid2 = fork();
		
		if(pid2 < 0){													//Check creation of child process 
			perror("Error creating process!");
			return 1;
		}
		
		if(pid2 > 0){
			printf("Numero: ");
			scanf("%d",&ptr->numero);

			printf("Nome: ");
			scanf("%s",ptr->nome);

			for(int i = 0; i<NR_DISC;i++){
				printf("Nota %d: ",i+1);
				scanf("%d",&ptr->disciplinas[i]);
			}
		
			ptr->flag = 1;
			
		}else{															//Child2
			while(!ptr->flag);											// While flag is 0 keep waiting
		
			float total = 0;
			for (int i = 0; i < NR_DISC; i++) {							// Sum the values
				total = total + ptr->disciplinas[i];	
			}
			
			printf("AVERAGE: %.2f\n",(total/NR_DISC));
		
		
        exit(0);
	}
	
	}else{																//Child1
		while(!ptr->flag);												// While flag is 0 keep waiting
		
		int max = 0;
		int min = 20;
		
		for (int i = 0; i < NR_DISC; i++) {								// search for highest and the lowest grade
			if(ptr->disciplinas[i] > max){
				max = ptr->disciplinas[i];
			}
			if(ptr->disciplinas[i] < min){
				min = ptr->disciplinas[i];
			}		
		}
		
		printf("MAX: %d\n",max);										// Print the max value
		printf("MIN: %d\n",min);										// Print the min value
        exit(0);
	}
	

	if (munmap(ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Erro! -  Unmap the shared memory segment");	
		exit(1);
	}
	close(fd);
	if (shm_unlink("/shmtest") < 0) {	// Unlink the shared memory segment (removes the shared memory segment from the file
		perror("Erro! - unlink the shared memory segment"); 
		exit(1);
	}
	
	

return 0;
}
