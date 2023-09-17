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

#define SIZE 1000000

typedef struct{
    int num1;
	int num2;
}nums;


int main(int argc, char *argv[]){
	int fd, trunk, unlink, unmap, close_fd, data_size = sizeof(nums);
	nums *ptr;

	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if (fd == -1) {
		perror("shm_open failed!\n");
		return 1;
	}

	trunk = ftruncate(fd, data_size);
	if (trunk == -1) {
		perror("ftruncate failed!\n");
		return 1;
	}

	ptr = (nums *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED) {
		perror("mmap failed!\n");
		return 1;
	}

	ptr->num1 = 10000;	
	ptr->num2 = 500;
		
	pid_t pid = fork();
	
	if(pid < 0){															//Check creation of child process 
		perror("Error creating process!");
		return 1;
	}
	
	if(pid > 0){
		for(int i=0;i<SIZE;i++){
            ptr->num1--;
            ptr->num2++;
        }	
	}else{
		for(int i=0;i<SIZE;i++){
            ptr->num1++;
            ptr->num2--;
        }
        exit(0);
	}
	
	wait(NULL);

    printf("Final value\nFirst integer: %d\nSecond integer: %d\n", ptr->num1, ptr->num2);

	unmap = munmap(ptr, data_size);
	if (unmap == -1) {
		perror("unmap failed!\n");
		return 1;
	}

	close_fd = close(fd);
	if (close_fd == -1) {
		perror("close fd failed!\n");
		return 1;
	}
	
	unlink = shm_unlink("/shmtest");
	if (unlink == -1) {
		perror("shm_unlink failed!\n");
		return 1;
	}

	return 0;
}