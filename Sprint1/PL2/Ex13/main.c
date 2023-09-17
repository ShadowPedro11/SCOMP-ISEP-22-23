#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define MACHINES 4

int main(){
	int pieces = 1000;													//Number of pieces in the Factory
	int notfOpen = 0;													//Open Notification
	int notfColse = 1;													//Close Notification
	
	int piecesPerMachine[MACHINES] = {5,5,10,100};						//Number of pieces worked per machine
	 
    int notf_pipes[4][2];												//Notification pipe
    int trnsf_pipes[4][2];												//Transfer pipe
    

	for (int i = 0; i < 4; i++) {										// Create pipess 
		if (pipe(notf_pipes[i]) < 0 || pipe(trnsf_pipes[i]) < 0 ) {
			perror("Pipe failed");
			exit(1);
		}
	}

	pid_t pid_array[MACHINES];											//creation of an array of pid_t to create all child's
	int status_array[MACHINES];


	    
	for(int i = 0; i < MACHINES;i++){									//fori cycle to create all childs
		
		pid_array[i] = fork();											//create cycle at position i of the pid_array

		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");
			return 1;
		}

		if(pid_array[i] == 0){											//In the child process
			
			if(i==0){													//M1 - Machine 1
				close(notf_pipes[i][0]);
				close(trnsf_pipes[i][0]);
				
				int j = 1;
				
				while(pieces >= piecesPerMachine[i]){												// While cycle that occurs while pieces given are bigger than piecesPerMachine[i](number of pieces of Machine 1)
					write(trnsf_pipes[i][1],&piecesPerMachine[i],sizeof(piecesPerMachine[i]));		// Write on transfer pipe the pieces transfered to next machine
					pieces = pieces - piecesPerMachine[i];											// Sub the number of pieces transfered
					write(notf_pipes[i][1],&notfOpen,sizeof(notfOpen));								// Write on Notification pipe that pieces were sent
					printf("M1-%d: %d pieces were sent from M1 to M2! \n",j,piecesPerMachine[i]);	// Print the actin
					j++;
				}
				
				write(notf_pipes[i][1],&notfColse,sizeof(notfColse));	// At the end write at notification pipe that the work is done on this machine
				
				close(notf_pipes[i][1]);
				close(trnsf_pipes[i][1]);
			}else if(i == 1){											//M2 - Machine 2
				close(notf_pipes[i][0]);
				close(trnsf_pipes[i][0]);
				
				int stack = 0;
				int value;											
				int state = 1;
				int j = 1;
				int w = 1;
				
				read(notf_pipes[i-1][0], &state, sizeof(state));		// Read in the notification pipe if this machine is available to work
				while(state == 0){										// While cycle that ocurrs while the state read on notification pipe is 0 (open)
					read(trnsf_pipes[i-1][0],&value,sizeof(value));		// Read on transfered pipe the pieces passed to this machine
					printf("M2-%d: %d pieces recived.\n",j,value);		// Present the pieces received
					stack = stack + value;								// Add the number of pieces received to stack of this machine 
					
					if(stack >= piecesPerMachine[i]){													//Check if the value on stack is bigger than piecesPerMachine[i] -> quantity to sent to next machine
						printf("M2-%d: %d pieces transfered to M3.\n",w,piecesPerMachine[i]);			//Print the action
						write(trnsf_pipes[i][1],&piecesPerMachine[i],sizeof(piecesPerMachine[i]));		//Write on transfer pipe the pieces transfered to next machine
						write(notf_pipes[i][1],&notfOpen,sizeof(notfOpen));								//Write on Notification pipe that pieces were sent
						stack = stack - piecesPerMachine[i];											//Sub at stack the number of pieces transfered
						w++;
					}
					
					read(notf_pipes[i-1][0], &state, sizeof(state));	// Read and update the state of machine
					j++;
				}
				write(notf_pipes[i][1],&notfColse,sizeof(notfColse));	// At the end write at notification pipe that the work is done on this machine

				close(notf_pipes[i][1]);
				close(trnsf_pipes[i][1]);
			}else if(i == 2){											//M3 - Machine 3
				close(notf_pipes[i][0]);
				close(trnsf_pipes[i][0]);
				
				int stack = 0;
				int value;
				int state = 1;
				int j = 1;
				int w = 1;
				
				read(notf_pipes[i-1][0], &state, sizeof(state));		//Read in the notification pipe if this machine is available to work
				while(state == 0){										// While cycle that ocurrs while the state read on notification pipe is 0 (open)
					read(trnsf_pipes[i-1][0],&value,sizeof(value));			// Read on transfered pipe the pieces passed to this machine
					printf("M3-%d: %d folded pieces recived.\n",j,value);	// Present the pieces received
					stack = stack + value;									// Add the number of pieces received to stack of this machine 
					
					
					if(stack>=piecesPerMachine[i]){						//Send to M4				//Check if the value on stack is bigger than piecesPerMachine[i] -> quantity to sent to next machine
						printf("M3-%d: %d pieces transfered to M4.\n",w,piecesPerMachine[i]);		//Print the action		
						write(trnsf_pipes[i][1],&piecesPerMachine[i],sizeof(piecesPerMachine[i]));	//Write on transfer pipe the pieces transfered to next machine
						write(notf_pipes[i][1],&notfOpen,sizeof(notfOpen));							//Write on Notification pipe that pieces were sent
						stack=stack-piecesPerMachine[i];											//Sub at stack the number of pieces transfered
						w++;
					}

					read(notf_pipes[i-1][0], &state, sizeof(state));	//Read and update the state of machine
					j++;
				}
				write(notf_pipes[i][1],&notfColse,sizeof(notfColse));	// At the end write at notification pipe that the work is done on this machine

				close(notf_pipes[i][1]);
				close(trnsf_pipes[i][1]);
			}else if(i == 3){											//M4 - Machine 4
				close(notf_pipes[i][0]);
				close(trnsf_pipes[i][0]);
				
				int stack = 0;
				int value;
				int state = 1;
				int j = 1;
				int w = 1;
				
				read(notf_pipes[i-1][0], &state, sizeof(state));		//Read in the notification pipe if this machine is available to work
				while(state == 0){										// While cycle that ocurrs while the state read on notification pipe is 0 (open)
					read(trnsf_pipes[i-1][0],&value,sizeof(value));			// Read on transfered pipe the pieces passed to this machine
					printf("M4-%d: %d welded pieces recived.\n",j,value);	// Present the pieces received
					stack = stack + value;									// Add the number of pieces received to stack of this machine 
					
					if(stack>=piecesPerMachine[i]){						//Send to M4				//Check if the value on stack is bigger than piecesPerMachine[i] -> quantity to sent to next machine
						printf("M4-%d: %d pieces transfered to StorageA.\n",w,piecesPerMachine[i]);	//Print the action
						write(trnsf_pipes[i][1],&piecesPerMachine[i],sizeof(piecesPerMachine[i]));	//Write on transfer pipe the pieces transfered to next machine
						write(notf_pipes[i][1],&notfOpen,sizeof(notfOpen));							//Write on Notification pipe that pieces were sent
						stack=stack-piecesPerMachine[i];											//Sub at stack the number of pieces transfered
						w++;
					}

					read(notf_pipes[i-1][0], &state, sizeof(state));	//Read and update the state of machine
					j++;
				}
				write(notf_pipes[i][1],&notfColse,sizeof(notfColse));	// At the end write at notification pipe that the work is done on this machine
				
				close(notf_pipes[i][1]);
				close(trnsf_pipes[i][1]);
			}
			exit(0);
		}
	}

	close(notf_pipes[MACHINES-1][1]);
	close(trnsf_pipes[MACHINES-1][1]);
				
	int storageA1 = 0;
	int state = 1;
	int j = 1;
	int value;
	read(notf_pipes[MACHINES-1][0], &state, sizeof(state));				//Read in the notification pipe if this machine is available to work
	while(state == 0){													//While cycle that ocurrs while the state read on notification pipe is 0 (open)
		read(trnsf_pipes[MACHINES-1][0],&value,sizeof(value));			//Read on transfered pipe the pieces passed to this machine
		printf("SA-%d: %d pieces received.\n",j,value);					//Present the pieces received
		if(value == piecesPerMachine[MACHINES-1]){						//If the value equal piecesPerMachine[MACHINES-1](100) add them to Storage
			storageA1 = storageA1 + piecesPerMachine[MACHINES-1];		//Add pieces
		}
		
		read(notf_pipes[MACHINES-1][0], &state, sizeof(state));			//Read and update the state of machine
		j++;
	}
	
	close(notf_pipes[MACHINES-1][0]);
	close(trnsf_pipes[MACHINES-1][0]);

	for(int i = 0; i < MACHINES; i++){									//Wait for all child process
		waitpid(pid_array[i], &status_array[i], 0);						
	}
	
	printf("Storage A1 -> %d\n",storageA1);								//Print Storage A value at the end of program
	
	
	
	
	return 0;
    }
