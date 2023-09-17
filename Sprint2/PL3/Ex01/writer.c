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

#include "ex1.h"

int main(int argc, char *argv[]){
	
	//shm_unlink("/shmtest");													// Unlink (delete) any existing shared memory segment with the same name
	
	int fd, data_size, trunk;
	student_info *ptr;
	
	data_size = sizeof(student_info);
	
	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);						// Create a new shared memory segment with read/write permissions, and truncate it to the size of student_info struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);																// Truncate the shared memory segment to the size of student_info 
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (student_info *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
																		// Prompt user for student information
	char student_name[50];
	printf("Enter student name: ");
	fgets(student_name, sizeof(student_name), stdin);
	
	if(student_name[strlen(student_name) -1] == '\n')
		student_name[strlen(student_name) -1] = '\0';
		
	int student_number;
	printf("Enter student number: ");
	scanf("%d", &student_number);
	
	while(getchar() != '\n');
	
	char student_address[50];
	printf("Enter student address: ");
	fgets(student_address, sizeof(student_address), stdin);
	
	if(student_address[strlen(student_address) -1] == '\n')
		student_address[strlen(student_address) -1] = '\0';
	
																		// Copy the inputted student information to the shared memory segment
	strcpy(ptr->name, student_name);
	ptr->number = student_number;
	strcpy(ptr->address, student_address);
	
	


return 0;
}
