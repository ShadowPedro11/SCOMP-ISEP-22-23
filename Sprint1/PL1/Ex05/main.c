#include <stdio.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
	
	pid_t p = fork();                                                   //Cria 1º processo
	
	if(p < 0){                                                          //verifica se o processo foi criado
        perror("Error creating Fork");                                  //
        return 1;                                                       //
        }
    
	if(p == 0){                                                         //Se p=0 então executa o processo filho
		printf("Son_1 -> Sleeping for 1 second.\n");                    //
		sleep(1);                                                       //sleep por 1 segundo
        exit(1);                                                        //exit do valor 1
	}else{                                                              //Se p > 0 então o processo filho já foi executado vai exucatar o processo pai
		pid_t p2 = fork();                                              //Cria 2º processo
		
		if(p2 < 0){                                                     //verifica se o processo foi criado
        perror("Error creating Fork");                                  //
        return 1;                                                       //
        }
		
		if(p2 == 0){                                                    //Se p2=0 então executa o processo filho
			printf("Son_2 -> Sleeping for 2 second.\n");                //
			sleep(2);                                                   //sleep por 2 segundos
			exit(2);                                                    //exit do valor 2
		}else{                                                          //Se p2 > 0 então o processo filho já foi executado vai exucatar o processo pai 
			int statusChildOne, statusChildTwo;                         //Criação de dois valores int para guardar o status dos processos
			
			waitpid(p, &statusChildOne, 0);                             //função waitpid com p e options(0) que bloqueia até que o processo indicado termine
			if(WIFEXITED(statusChildOne)){                              //WIFEXITED permite verificar se o filho terminou normalmente	
                printf("Son 1: %d\n",WEXITSTATUS(statusChildOne));    	//print valor de saida do processo
			}
			
            waitpid(p2, &statusChildTwo, 0);                            //função waitpid com p2 e options(0) que bloqueia até que o processo indicado termine
            if(WIFEXITED(statusChildTwo)){                              //WIFEXITED permite verificar se o filho terminou normalmente	
                printf("Son 2: %d\n",WEXITSTATUS(statusChildTwo));     	//print valor de saida do processo
			}
		}
	
	}
	
	return 0;
}
