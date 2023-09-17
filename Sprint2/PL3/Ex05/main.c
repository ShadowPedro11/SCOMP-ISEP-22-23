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
#include <signal.h>

#define TRUE 1
#define FALSE 0
#define NUMBER_OPERATIONS 1000000

int available;

typedef struct {
    int num;
}shared_int;

void handle_USR1(int signo, siginfo_t *sinfo, void *context) {
    available = TRUE;
}

int main() {
    int fd, trunk, data_size = sizeof(shared_int);
	shared_int *ptr;

	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);		// Create a new shared memory segment with read/write permissions, and truncate it to the size of share struct
	if (fd == -1) {
		perror("shm_open failed!\n");
		return 1;
	}

	trunk = ftruncate(fd, data_size);		// Truncate the shared memory segment to the size of share
	if (trunk == -1) {
		perror("ftruncate failed!\n");
		return 1;
	}

	ptr = (shared_int *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);		// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED) {
		perror("mmap failed!\n");
		return 1;
	}

    ptr->num = 100;

    int status;
    
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask); /* No signals blocked */
    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    available = TRUE;

    pid_t pid = fork();

    if (pid == -1){
        perror("Fork failed!\n");

        if (munmap(ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		    perror("Error! -  Unmap the shared memory segment");	
		    exit(1);
	    }

	    close(fd);

	    if (shm_unlink("/shmtest") < 0) {	// Unlink the shared memory segment (removes the shared memory segment from the file
		    perror("Error! - unlink the shared memory segment"); 
		    exit(1);
	    }
        return 1;
    } 

    if (pid > 0) {
        //parent

        for (int i = 0; i < NUMBER_OPERATIONS; i++) {
            if (!available) pause();
            available = FALSE;
            ptr->num++;
            printf("Pai %d\n", i);
            kill(pid, SIGUSR1);
        //}

        //for (int i = 0; i < NUMBER_OPERATIONS; i++) {
            if (!available) pause();
            available = FALSE;
            ptr->num--;
            printf("Pai2 %d\n", i);
            kill(pid, SIGUSR1);
        }
        wait(&status);
        if (WIFEXITED(status)){
            printf("Final value of shared int = %d\n", ptr->num);
        } else {
            printf("Child didn't finish normally");
        }

    } else {
        //child

        for (int i = 0; i < NUMBER_OPERATIONS; i++) {
            if (!available) pause();
            available = FALSE;
            ptr->num++;
            printf("Filho %d\n", i);
            kill(getppid(), SIGUSR1);
        //}

        //for (int i = 0; i < NUMBER_OPERATIONS; i++) {
            if (!available) pause();
            available = FALSE;
            ptr->num--;
            printf("Filho2 %d\n", i);
            kill(getppid(), SIGUSR1);
        }

        exit(EXIT_SUCCESS);
    }

    if (munmap(ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Error! -  Unmap the shared memory segment");	
		exit(1);
	}

	close(fd);

	if (shm_unlink("/shmtest") < 0) {	// Unlink the shared memory segment (removes the shared memory segment from the file
		perror("Error! - unlink the shared memory segment"); 
		exit(1);
	}

    return 0;
}