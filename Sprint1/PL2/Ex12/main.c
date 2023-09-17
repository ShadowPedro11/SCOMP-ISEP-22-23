#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CHILDS_NUM 5

/*Spawns a CHILD_NUM number of child processes and stores the pids 
in an array. Child process return its index and exits the function.
Parent process continues the for cicle and at the end returns -1.*/
int spawn_childs(pid_t *pid_array) {
    for (int i = 0; i < CHILDS_NUM; i++){
        pid_array[i] = fork();

        if (pid_array[i] == -1){
            printf ("Fork failed in child %d\n", i);
            exit(EXIT_FAILURE);
        }
        if (pid_array[i] == 0) return i;  
    }
    return -1;
}

/*Struct that stores the information of a Product.*/
typedef struct {
    int barcode;
    float price;
    char name[24];
} Product;

/*This function searches for a product with a certain barcode in the Product database 
and returns the struct of the matching barcode.*/
Product search_product(int barcode_requested, Product prod_database[3], int db_size){
    for (int i = 0; i < db_size ; i++) {
        if(prod_database[i].barcode == barcode_requested) return prod_database[i];
    }
}

/*This function establishes two pipes, one for each direction (parent-child and child-parent).
Then spawns 5 childs (simulating the barcode readers). 
The parent creates some products and puts them in an array that simulates the data base. Then 
enters a while loop that is limited to 15 iterations which is the amount of executions that the 
children will do in total. Inside the while, the parent reads the barcode that was requested 
through the pipe from a child and searches for the matching product using the function 
"search_product()". The struct of the requested product is then sent to the child.
Each child process will request a product 3 times, because of the for loop. Each barcode 
request is random, using the function "rand()". This barcode is sent to parent and then the 
matching product is read from the pipe and printed.*/
int main(){
    int fd_parent_child[2];
    int fd_child_parent[2];
    pid_t pid_array[CHILDS_NUM];

    if (pipe(fd_parent_child) == -1) {
        perror("Pipe1 failed!\n");
        return 1;
    }

    if (pipe(fd_child_parent) == -1) {
        perror("Pipe2 failed!\n");
        return 1;
    }

    int index = spawn_childs(pid_array);

    if (index == -1){
        //parent 

        close(fd_parent_child[0]);      //Close read
        close(fd_child_parent[1]);      //Close write

        Product p1, p2, p3;

        p1.barcode = 1;
        p1.price = 0.20;
        strcpy(p1.name, "Pão");  

        p2.barcode = 2;
        p2.price = 0.94;
        strcpy(p2.name, "Massa Esparguete"); 

        p3.barcode = 3;
        p3.price = 4.19;
        strcpy(p3.name, "Azeite"); 

        Product prod_database[3];
        prod_database[0] = p1;
        prod_database[1] = p2;
        prod_database[2] = p3;

        int nr_reads = 15;

        while (nr_reads > 0){
            int barcode_requested;
            //Read the barcode requested
            if (read(fd_child_parent[0], &barcode_requested, sizeof(int)) == -1) {
                printf("Read failed!\n");
                exit(EXIT_FAILURE);
            }

            Product prod_requested;
            //Search for the matching product
            prod_requested = search_product(barcode_requested, prod_database, 3);

            //Send the requested product
            if (write(fd_parent_child[1], &prod_requested, sizeof(Product)) == -1) {
                printf("Write failed!\n");
                exit(EXIT_FAILURE);
            }
           nr_reads--;
        }

        close(fd_parent_child[1]);      //Close write
        close(fd_child_parent[0]);      //Close read

    } else {
        close(fd_parent_child[1]);      //Close write
        close(fd_child_parent[0]);      //Close read

        Product prod_request;

        for (int i = 0; i < 3; i++) {
            srand(time(NULL) + getpid());
            //Generate a random barcode (between 1 and 3)
            int barcode = (rand() % 3) + 1;

            //Send the barcode
            if (write(fd_child_parent[1], &barcode, sizeof(int)) == -1){
                printf("Write failed!\n");
                exit(EXIT_FAILURE);
            }

            //Read the requested product
            if (read(fd_parent_child[0], &prod_request, sizeof(Product)) == -1) {
                printf("Read failed!\n");
                exit(EXIT_FAILURE);
            }

            printf("Barcode Reader %d: --Barcode: %d --Price: %.2f€ --Name: %s \n", index + 1, prod_request.barcode, prod_request.price, prod_request.name);
        }

        close(fd_parent_child[0]);      //Close read
        close(fd_child_parent[1]);      //Close write
        exit(EXIT_SUCCESS);
    }

    return 0;
}