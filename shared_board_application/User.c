#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wait.h>
#include <time.h>
#include <string.h>

#define ROWS 20
#define COLUMNS 10

typedef struct {
    char board[ROWS][COLUMNS][50];
}sharedBoard;

int getIndex(int row, int col){

	int index = 0;

	row--;
	col--;

	while(row != 0){
		index +=10;
		row--;
	}
	index = index + col;

	return index;

}

void clean_console()
{
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void move_postIt(sem_t **semaphores, sharedBoard *board)
{
	printf("\n\n\nChange posti menu\n\n\n");

	int row_before;
	int row_after;
	int column_before;
	int column_after;

	printf("Select a row to move it from:\n");
	scanf("%d", &row_before);

	if (row_before < 1 || row_before >= ROWS)
	{
		printf("The row number should be between 1 and 20\n");
		return;
	}

	printf("Select a column to move it from:\n");
	scanf("%d", &column_before);
	
	if (column_before < 1 || column_before >= COLUMNS)
	{
		printf("The column number should be between 1 and 10\n");
		return;
	}

	printf("Select a row to move it to:\n");
	scanf("%d", &row_after);

	if (row_after < 1 || row_after >= ROWS)
	{
		printf("The row number should be between 1 and 20\n");
		return;
	}

	printf("Select a column to move it to:\n");
	scanf("%d", &column_after);

	if (column_after < 1 || column_after >= COLUMNS)
	{
		printf("The column number should be between 1 and 10\n");
		return;
	}


	int index_before = getIndex(row_before, column_before);
	int index_after = getIndex(row_after, column_after);

	sem_wait(semaphores[index_after]);

	if (strcmp(board->board[row_after - 1][column_after - 1], "EMPTY" ) != 0)
	{
		printf("There is a postIt in that cell, do you want to overwrite?\n");
		char awnser[4];
		  
		scanf("%s", awnser);

		if (strcmp("no", awnser) == 0)
		{
			sem_post(semaphores[index_after]);
			return;
		}
	}

	sem_wait(semaphores[index_before]);

	strcpy(board->board[row_after - 1][column_after - 1], board->board[row_before - 1][column_before - 1]);
	strcpy( board->board[row_before - 1][column_before - 1], "EMPTY");

	sem_post(semaphores[index_after]);
	sem_post(semaphores[index_before]);
}

void delete_postIt(sem_t **semaphores, sharedBoard *board)
{
	printf("\n\n\nDelete posti menu\n\n\n");

	int row;
	printf("Select a row:");
	scanf("%d", &row);

	if (row < 1 || row >= ROWS)
	{
		printf("The row number should be between 1 and 20\n");
		return;
	}

	int column;
	printf("Select a column:");
	scanf("%d", &column);

	if (column < 1 || column >= ROWS)
	{
		printf("The column number should be between 1 and 10\n");
		return;
	}

	int index = getIndex(row, column);

	sem_wait(semaphores[index]);

    strcpy(board->board[row - 1][column - 1], "EMPTY");

	sem_post(semaphores[index]);
}

int main()
{

	int fd, data_size, trunk;
	sharedBoard *board;

	data_size = sizeof(sharedBoard);

	fd = shm_open("/shmtest", O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it to the size of barrier struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);												// Truncate the shared memory segment to the size of barrier
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}

	board = (sharedBoard *) mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (board == MAP_FAILED || board == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}


	sem_t *semaphores[ROWS*COLUMNS];
	char sem_name[20];

	for (int i = 0; i < (ROWS*COLUMNS); i++) {

		sprintf(sem_name, "/semaphore%d", i);
		semaphores[i] = sem_open(sem_name, 0);

		if(semaphores[i] == SEM_FAILED){
			printf("Error creating the semaphore!\n");
			exit(EXIT_FAILURE);
		}

	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	sem_t *exit_board = sem_open("board_exit", O_CREAT, 0644, 0);

	int choice;

    do {
        printf("Menu:\n");
        printf("1. Edit Post It\n");
        printf("2. Move Post It\n");
        printf("3. Delete postIT\n");
        printf("4. Show Board\n");
        printf("5. Exit\n");
        printf("6. Shut down board\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

		int row, col;

        switch (choice) {
            case 1:


				while (1) {
					printf("\n\n\n");
					printf("Enter a row : ");
					scanf("%d", &row);

					if (row >= 1 && row <= ROWS) {
						break;
					}

					printf("Error: Row must be between 1 and 20.\n");
				}

				while (1) {
					printf("Enter a column : ");
					scanf("%d", &col);

					if (col >= 1 && col <= COLUMNS) {
						break;
					}

					printf("Error: Row must be between 1 and 10.\n");
				}

				printf("\n\n\n");
				printf("You entered row %d and column %d.\n", row, col);

				int index = getIndex(row,col);

				sem_wait(semaphores[index]);
				printf("Actual:%s \n", board->board[row-1][col-1]);

				char new_string[50];
				printf("Enter a new string: ");
				scanf("%s", new_string);

				strcpy(board->board[row-1][col-1], new_string);


				sem_post(semaphores[index]);

                clean_console();
                break;
            case 2:
				move_postIt(semaphores, board);
                clean_console();
                break;
			case 3:
				delete_postIt(semaphores, board);
				clean_console();
				break;
            case 4:
				printf("\n\n\n\n\n");
                for (int i = 0; i < ROWS; i++) {
					for (int j = 0; j < COLUMNS; j++) {
						printf("%6s ", board->board[i][j]);
					}
					printf("\n");
				}
				printf("\n\n\n\n\n");
                break;
			case 6:
				printf("Shuting down shared board\n");
				sem_post(exit_board);
            case 5:
                printf("Exiting program.\n");
				sleep(1);
				clean_console();
                break;
				        
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }

    } while (choice != 5 && choice != 6);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






	if (munmap(board, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Erro! -  Unmap the shared memory segment");
		exit(1);
	}
	close(fd);



	return 0;
}



