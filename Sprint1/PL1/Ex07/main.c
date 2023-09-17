#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#define ARRAY_SIZE 1000

int count(int initial, int final,int num, int *array){					//Função que calcula o nº de ocorrencia do valor n
	int returnValue=0;													//num array desde o indice "initial" até ao indice "final" 
	
	for(int i = initial; i < final; i++){
			if (*(array+i) == num){
				returnValue++;
			}
		}
	
	return returnValue;
}


int main (){
	int numbers[ARRAY_SIZE];	                 						/* array to lookup */
	int n;																/* the number to find */
	time_t t;															/* needed to initialize random number generator (RNG) */
	int i;
  
	srand ((unsigned) time (&t));										/* intializes RNG (srand():stdlib.h; time(): time.h) */
  
	for (i = 0; i < ARRAY_SIZE; i++){									/* initialize array with random numbers (rand(): stdlib.h) */
	  numbers[i] = rand () % 10000;
	}
    
	n = rand () % 10000;												/* initialize n */
  
  	int half = ARRAY_SIZE >> 1;											//Divide ARRAY_SIZE por 2 (uso de operação bitwise)
  	int status;
  	
	pid_t p = fork();                                                   //Cria 1º processo
	if(p < 0){                                                          //verifica se o processo foi criado
		perror("Error creating Fork");                                  //			
		return 1;                                                       //
	}																	//
	
	int res1,res2;
	
	if(p == 0){															//Child
		res1 = count(0,half,n,numbers);									//Chama a função count e guarda o valor em res1
		printf("%d was found: %d times by child process.\n", n, res1);	//print dos resultados obtidos
		exit(res1);														//Exit do valor obtido pela função
		
    }else{																//Father
		res2 = count(half,ARRAY_SIZE,n,numbers);						//Chama a função count e guarda o valor em res2
		printf("%d was found: %d times by parent process.\n", n, res2);	//print dos resultados obtidos
    }
    	
	wait(&status);														//Uso de wait porque existe apenas um processo filho

    if(WIFEXITED(status)){												//Verifica se o processo filho terminou corretamente
		printf("==================================================================\n");
		printf("%d times found by parent process.\n", res2);
        printf("%d times found by child process.\n", WEXITSTATUS(status));
		printf("Number of %d occurrences in the entire array: %d\n",n,WEXITSTATUS(status)+res2);
        printf("==================================================================\n");
        }
        
  return 0;
}
