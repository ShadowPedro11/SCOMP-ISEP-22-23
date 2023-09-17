#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#define ARRAY_SIZE 50000
#define PRODUCT_UNITS 20
#define CHUNK_SIZE 5000

typedef struct{															// Sale Struct
 int customer_code;
 int product_code;
 int quantity;
}Sale;

void printSales(Sale *sales, int size) {								// Method to print Sales
  for (int i = 0; i < size; i++) {
	  if(sales[i].quantity > PRODUCT_UNITS){
    printf("Sale %d:\n", i + 1);
    printf("Customer code: %d\n", sales[i].customer_code);
    printf("Product code: %d\n", sales[i].product_code);
    printf("Quantity: %d\n", sales[i].quantity);
    printf("\n");
}
  }
}

int main(){
    Sale sales[ARRAY_SIZE];												//Sales array
    int products[ARRAY_SIZE];											//products array
    
	
	time_t t;															/* needed to initialize random number generator (RNG) */
	int i;

	srand ((unsigned) time (&t));										/* intializes RNG (srand():stdlib.h; time(): time.h) */

	for (i = 0; i < ARRAY_SIZE; i++){									/* initialize arrays with random numbers (rand(): stdlib.h) */
		sales[i].customer_code = rand() % 1000;
        sales[i].product_code = rand() % 1000;
        sales[i].quantity = rand() % 50;
	}
     
    int size = 10;														//size -> number of childs
    
    int fd[size][2];

	for (int i = 0; i < size; i++) {									// Creates 10 pipes 
		if (pipe(fd[i]) < 0) {
			perror("Pipe failed");
			exit(1);
		}
	}

	pid_t pid_array[size];												//creation of an array of pid_t to create all child's
	int status_array[size];

	for(i = 0; i < size;i++){											//fori cycle to create all childs

		pid_array[i] = fork();											//create cycle at position i of the pid_array

		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");
			return 1;
		}

		if(pid_array[i] == 0){											//In the child process

			close(fd[i][0]);											//close read pipe

			int initial = i*CHUNK_SIZE;									//initial value to check
			int end = initial+CHUNK_SIZE;								//last value to check
		
			for(int j = initial; j < end; j++){							//cycle to check the values beetween initial and end
				if (sales[j].quantity > PRODUCT_UNITS) {					//check if sales quantity is bigger than PRODUCT_UNITS(20)
                    write(fd[i][1], &sales[j].product_code, sizeof(int));	//if quantity is bigger write in pipe
                }
			}
			
			close(fd[i][1]);											//close write pipe							
	
			exit(0);
		}
	}

	for(i = 0; i < size; i++){
		waitpid(pid_array[i], &status_array[i], 0);						//Wait childs to end
	}

	 int products_count = 0;

	for(i = 0; i < size; i++){

		close(fd[i][1]);												//close write pipe
		
		int product_code;
        while (read(fd[i][0], &product_code, sizeof(int)) > 0) {		//while cycle read the values in pipe
            products[products_count] = product_code;					//register in array the respective product code
            products_count++;
        }
        close(fd[i][0]);												//close open pipe
	}
	
	//printSales(sales,ARRAY_SIZE);
	
	printf("Products sold more than %d units in a single sale:\n", PRODUCT_UNITS);
    for (int i = 0; i < products_count; i++) {							//Cycle to print the products
        printf("%d: %d\n",i+1, products[i]);
    }
	
	return 0;
    }
