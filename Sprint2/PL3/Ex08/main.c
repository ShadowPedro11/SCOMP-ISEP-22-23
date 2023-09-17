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

#define NR_CHILDS 10

typedef struct {
    char files_path[NR_CHILDS][120];
    char words[NR_CHILDS][12];
    int nr_occ[NR_CHILDS];
    int wait;
}shared;

/*This function will create an area of shared memory and then fill it with a path to a text file, a word to search and initialize the number
of occurrences of the word in the text file with zero for each child. Then the parent process creates ten childs. Each child will search for 
the assigned word and count the number of occurrences it finds in the text file. The parent will wait for every child to finish its execution 
and then it will print the number of occurrences each child found for their word.*/
int main() {
    int fd, trunk, data_size = sizeof(shared);
	shared *ptr;

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

	ptr = (shared *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);		// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED) {
		perror("mmap failed!\n");
		return 1;
	}

    //Fill the shared memory area with the information for each child process
    strcpy(ptr->files_path[0], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c1.txt");
    strcpy(ptr->files_path[1], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c2.txt");
    strcpy(ptr->files_path[2], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c3.txt");
    strcpy(ptr->files_path[3], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c4.txt");
    strcpy(ptr->files_path[4], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c5.txt");
    strcpy(ptr->files_path[5], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c6.txt");
    strcpy(ptr->files_path[6], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c7.txt");
    strcpy(ptr->files_path[7], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c8.txt");
    strcpy(ptr->files_path[8], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c9.txt");
    strcpy(ptr->files_path[9], "/home/theblitz/SCOMP/scomp_22-23_2dj_1211131_1211128/Sprint2/PL3/Ex08/text_files/file_c10.txt");

    strcpy(ptr->words[0], "the");
    strcpy(ptr->words[1], "Elephant");
    strcpy(ptr->words[2], "Tree");
    strcpy(ptr->words[3], "Cat");
    strcpy(ptr->words[4], "Dog");
    strcpy(ptr->words[5], "Bicycle");
    strcpy(ptr->words[6], "Giraffe");
    strcpy(ptr->words[7], "Banana");
    strcpy(ptr->words[8], "Rain");
    strcpy(ptr->words[9], "Ocean");

    ptr->nr_occ[0] = 0;
    ptr->nr_occ[1] = 0;
    ptr->nr_occ[2] = 0;
    ptr->nr_occ[3] = 0;
    ptr->nr_occ[4] = 0;
    ptr->nr_occ[5] = 0;
    ptr->nr_occ[6] = 0;
    ptr->nr_occ[7] = 0;
    ptr->nr_occ[8] = 0;
    ptr->nr_occ[9] = 0;
    
    ptr->wait = 1;

    pid_t pid_array[NR_CHILDS];
    int status_array[NR_CHILDS];

    for (int i = 0; i < NR_CHILDS; i++) {
        pid_array[i] = fork();

        if (pid_array[i] == -1){
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

        if (pid_array[i] == 0) {
            //child

            FILE *ptr_file = fopen(ptr->files_path[i], "r");        //Open file

            char buff[100];

            if (ptr_file != NULL){
                
                /*fscanf() will get each word from the file and put it in the buff. Then
                the strcmp() funtion checks if the buff word equals the struct's word.
                This will be executed until the end of the file.*/
                while (fscanf(ptr_file, "%s", buff) != EOF){
                    while (!ptr->wait);     //Does active wait to ensure that two processes aren't trying to write update shared memory variables.
                    ptr->wait = 0;
                    printf("buff = %s\n", buff);
                    if (strcmp(ptr->words[i], buff) == 0) ptr->nr_occ[i]++;
                    ptr->wait = 1;
                } 
            }

            exit(EXIT_SUCCESS);
        }
    }
    
    for (int i = 0; i < NR_CHILDS; i++) {               //Wait for all childs
        waitpid(pid_array[i], &status_array[i], 0);
    }
    
    for (int i = 0; i < NR_CHILDS; i++) {       //Print the number of occurrences each child found
        if (WIFEXITED(status_array[i])){
            printf("Child %d found the word '%s' in file c%d %d times.\n", i+1, ptr->words[i], i+1, ptr->nr_occ[i]);
        }
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