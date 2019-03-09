/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */
 /*Edited by lexus uhlir
 to compile: gcc -o shell shell.c
 or clang shell.c -o shell for macs
 to execute: ./shell
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

/*setHistory will take the last command and put it into the history array*/
/*params include the history array  (char **history), the command (char **args) and the
size of the array (int *size)*/
void setHistory(char **history, char **args, int *size) {
	/*used for the for loop*/
	int j;
	for(j=0; j<*size;j++) {
		history[j]= strdup(args[j]);
}

}
/*printArray will print the given array
params include the array needed to be printed (char **array) and its size (int size)*/
void printArray(char **array, int size) {
		int i;
		/*if the array is not empty, print it*/
		if(array[0]!= 0) {
		for(i=0;i<size;i++) {
			printf("osh>>%s ",array[i]);
		}
	}
	/*if the array is empty, its most likely !! so print the error*/
		if(array[0]==NULL) {
			printf("osh> No commands in history");
		}
		printf("\n");

}
/*tokenizer will take the command line string and turn it into (char **args)
params also include char **history for printing later, int *status for the waitpaid
int *buffer for the history buffer triggered by !!, and int *size which is the size of the array*/
void tokenizer(char **args,char **history, int *status, int *buffer, int *size) {
	/*for looping*/
	int i=0;
	/*for the tokenizer*/
	char *tokens;
	/*for the command line string*/
	char command[MAX_LINE/2 + 1];
	/*for memory allocation*/
	size_t len;
	/*to check for control commands that should end the program*/
	int control;

		/*get new command from user*/
	 		control =scanf("%[^\n]",command);
			/*get the length for allocation*/
			len = strlen(command);
			/*chop of the \n*/
			getchar();
			/*split into tokens*/
			tokens = strtok(command," ");
			while(tokens!=NULL)
		{
		   args[i] = strdup(tokens);
			 tokens= strtok(NULL," ");
			 i++;
		}
		/*the array needs to end in null in order to work for execvp*/
			 args[i] = NULL;
			/*if they choose to exit, let them*/
			if(i>0) {
		if(strcmp(args[0],"exit") ==0 ){
			printf("osh>Bye\n");
			exit(0);

		}
		if(control == EOF){
			printf("Bye\n");
			exit(0);

		}
	}
		/*if they do !! they want to execute their last command*/
		if(i>0) {
		if(strcmp(args[0],"!!") ==0){
			/*echo the command for the user*/
			printArray(history,i);
			*buffer =1;
		}
		else {
			*buffer =0;
		}
	}
		/*if they have the &, they want to run the parent
		and child process concurrently*/
		if(i>1) {
		if(strcmp(args[(i -1)],"&") ==0){
			*status =1;
		}
	}
	/*set the size for the array for later use*/
	*size = i;

	}

/*main function takes no arguments. it runs the entire program*/
int main(void)
	{
			/* command line (of 80) has max of 40 arguments */
		char *args[MAX_LINE/2 + 1];
		/*history will store the last command from the user*/
		char *history[MAX_LINE/2 + 1];
		/*should run keeps the loop going*/
    	int should_run=1;
			/*used for waitpid, when the status is changed the parent process
			run concurrently with the child*/
			int *status;
			/*buffer makes sure that history is only changed when !! is not enetered.
			it also allows the execvp function to run the history array instead of the normal*/
			int *buffer =0;
			/*size is the size of the command line array*/
			int *size=0;

			/*this loops until an error occurs*/
    	while (should_run){
						status=0;
						printf("osh>");
						fflush(stdout);
						/*tokeinze the command line*/
						tokenizer(args,history, &status, &buffer, &size);
					//if there is no entries skip the fork bc nothing to execute
					if(size != 0) {
        	/**
         	 * After reading user input, the steps are:
         	 * (1) fork a child process
         	 * (2) the child process will invoke execvp()
         	 * (3) if command includes &, parent and child will run concurrently
         	 */
					 /*(1)*/
					 	pid_t  pid;

     				pid = fork();

						if(pid <0) {
							printf("Fork failed");
							exit(0);
						}
				/*(2) execvp(char *command, char *params[])*/
				/*if you allow the buffer, then it will report*/
					else if(pid ==0) {
						if(buffer){
							if(execvp(history[0],history)) {
								printf("osh> No commands in history\n");
								}

						}
						else {
							if(execvp(args[0],args)) {
								should_run=0;
								printf("osh> Execvp error: command not found\n");

							}
						}

						}
				else {
					/*(3)*/
					/*check for & to see if the parent has to wait for the process to end */
					if(status ==0) {
					waitpid(pid, &status, 0);
				}
				}
			}
			/*create the history array*/
			if(buffer ==0) {
			setHistory(history,args,&size);
		}
		printf("%i\n",should_run);
		}

	return 0;
}
