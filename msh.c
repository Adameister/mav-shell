/*
 * Name: Adam Hudson
 * ID #: 1000991758
 * Programming Assignment 1
 * Description: Create msh to accept commands
 *				fork a child process and 
 *				execute those commands
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>



// Functions

/*	
	Function: handle_signal
	Parameters: integer signal representing the signal to kill a program
	Returns: A new line. This will allow the user to know that
			 their signal was accepted, but did not do anything
	Description: handle_signal ignores the ctrl+c and ctrl+z signals
				 given to the program by the user
				 to end the msh, the user must type exit or quit, to quit
*/
static void handle_signal (int sig){
	// program returns new line to quietly ignore signal
	printf("\n");
}

/*
	Function: storePid
	Parameters: an integer child_pid with the process ID of the child process
				an integer array called pidList containing the last 10 process IDs.
	Returns: nothing
	Description: storePid will simply update the current child process ID
				 for later refrence by the user. The oldest pid will fall off
				 the list after 10 processes
*/
void storePid( int child_pid, int pidList[] ){
	pidList[0] = pidList[1];
	pidList[1] = pidList[2];
	pidList[2] = pidList[3];
	pidList[3] = pidList[4];
	pidList[4] = pidList[5];
	pidList[5] = pidList[6];
	pidList[6] = pidList[7];
	pidList[7] = pidList[8];
	pidList[8] = pidList[9];
	pidList[9] = child_pid;
}

/*
	Function: showpid
	Parameters: An integer array containing the last 10 process IDs ran
				by the user.
	Returns: nothing
	Description: showpid will print the array pidList that contains the
				 last 10 process made by the user
*/
void showpid( int pidList[] ){
	int i;

	printf("Oldist pid:%d\n", pidList[0]);
	for (i = 1; i < 8; i++){
		printf("          :%d\n", pidList[i]);
	}
	printf("Newist pid:%d\n", pidList[9]);
}

/*
	Function: run_command
	Parameters: A charater pointer, command, that contains which command the
				user wants to execitue.
				opt1 - opt5 are the option flags the user wants to exeicute
				with/for the given command.
	Returns: nothing
	Description: run_command will try to run the command with the options given
				 by the user. The new varable "path" will contain the current path
				 to the command in question. If the path is incorrect, path will 
				 get over written with the new path to see if the command will then
				 exeicute. If the command never exicutes, the process will print
				 that the command is not found. 
*/
void run_command (char *command, char *opt1, char *opt2, char *opt3, char *opt4, char *opt5){
	char *current = "";
	char *bin = "/bin/";
	char *usrBin = "/usr/bin/";
	char *usrLocalBin = "/usr/local/bin/";
	// the memory of path is allocated to prevent seg faults
	char *path = malloc (strlen (bin) + 
						 strlen (usrBin) + 
						 strlen (usrLocalBin) + 
						 strlen (command) + 1);


	/*
		The path of the command must be concatinated with the
		command. For example, the "ls" command is in "/bin/ls"
		not in "/bin".
	*/
	strcpy(path, current);
	strcat(path, command);
	execl (path, command, opt1, opt2, opt3, opt4, opt5, NULL);

	strcpy(path, usrLocalBin);
	strcat(path, command);
	execl (path, command, opt1, opt2, opt3, opt4, opt5, NULL);

	strcpy(path, usrBin);
	strcat(path, command);
	execl (path, command, opt1, opt2, opt3, opt4, opt5, NULL);

	strcpy(path, bin);
	strcat(path, command);
	execl (path, command, opt1, opt2, opt3, opt4, opt5, NULL);

	free(path);

	// if there is a command, and the process didnt find the process
	// then print that the command was not found
	if ( strlen(command) > 0 ){
		printf("%s: Command not found.\n", command);
	}


}

/*
	Function: pass_input
	Parameters: A charater pointer, command, that contains which command the
				user wants to execitue.
				opt1 - opt5 are the option flags the user wants to exeicute
				with/for the given command.
				An integer array pidList that contains the last 10 process IDs ran
				by the user.
	Returns: An int of if the process succeeded or failed.
	Description: pass_input forks the parent process and gives the child process
				 the command wanted by the user. The child process will run the
				 function run_command while the parent process runs the function
				 storePid to update pidList with a new process ID.
*/
int pass_input (char *command, char *opt1, char *opt2, char *opt3, char *opt4, char *opt5,
				int pidList[]){
	int status;

	pid_t child_pid = fork();	// will return 0 upon success

	if ( child_pid == -1 ){		// if fork failed, the process will end and print that the fork failed
		perror("Fork error");
		exit(EXIT_FAILURE);
	} else if( child_pid == 0){	// the child process is given the refined input from the user to try and exe the command
		run_command(command, opt1, opt2, opt3, opt4, opt5);
		exit(EXIT_SUCCESS);		// even if the command is not found, the process will tell the user its not found, then exit
	} else{
		storePid(child_pid, pidList); // every fork, the process is stored
		waitpid( child_pid, &status, 0);		  // parent will wait for its child
		fflush(NULL);
	}

	return EXIT_SUCCESS;
}

// to run, type make, then ./msh
int main( void ){
	// var declaration
	char uInput[255];	// grabs the user input
	char *token;		// contains the entire tokenized string from uInput
	char *command;		// contains the user's command
	char *opt1 = NULL;	// contains the options the user wants to exe with command
	char *opt2 = NULL;	// thses are initalized to NULL to prevent unknown results
	char *opt3 = NULL;
	char *opt4 = NULL;
	char *opt5 = NULL;
	int pidList [10] = {0,0,0,0,0,0,0,0,0,0};	// the list of the last 10 process
												// made by the user. If process is 0
												// there has not been 10 process yet

	/*
		This block of code will handle the ctrl+c and ctrl+z
		signals given by the user.
		the parent process will recieve these signals and quietly
		ignore them.
	*/
	struct sigaction act;
	memset (&act, '\0', sizeof(act));
	act.sa_handler = &handle_signal;
	// for ctrl+c:
	if (sigaction(SIGINT, &act, NULL) < 0){
		perror ("sigaction: ");
		exit( EXIT_FAILURE );
	}
	// for ctrl+z:
	if (sigaction(SIGTSTP, &act, NULL) < 0){
		perror ("sigaction: ");
		exit( EXIT_FAILURE );
	}

	while(1){
		// program will always run unless the user wants to exit

		// always print the prompt for the next command
		printf("msh>");

		// fgets the input from the user. Max of 255 charaters
		fgets( uInput, 255, stdin );

		/*	
			disassemble the input from the user and break it in to
			6 parts, first part is the command, parts 2-6 are the options.
		*/
		token = strtok(uInput, " ");
		if( token != NULL ){
			/*	
				command will take in a \n from the token call.
				in order to fix this I need to run the strcspn function
				to cut off the string when it finds a \n.
				The token will also be cut off when there is a retun 
				statement or a null charater.
			*/	
			command = token;
			command[strcspn(command, "\n")] = 0;
			token = strtok(NULL, " \r\n\0");
		}
		if( token != NULL ){	
			opt1 = token;
			token = strtok(NULL, " \r\n\0");
		}
		if( token != NULL ){	
			opt2 = token;
			token = strtok(NULL, " \r\n\0");
		}
		if( token != NULL ){	
			opt3 = token;
			token = strtok(NULL, " \r\n\0");
		}
		if( token != NULL ){	
			opt4 = token;
			token = strtok(NULL, " \r\n\0");
		}
		if( token != NULL ){	
			opt5 = token;
			token = strtok(NULL, " \r\n\0");
		}

		/*
			This is my exit functionality.
			If the command is quit or exit, my program will quit.
			I made the code like this, so the user could exit the program
			even if there are spaces before or after the exit command.
		*/
		if(strncmp( command, "exit" , 4) == 0){
			printf("Exiting...\n");
			exit( EXIT_SUCCESS );	// exit the program
		}
		if(strncmp( command, "quit" , 4) == 0){
			printf("Quitting...\n");
			exit( EXIT_SUCCESS );	// exit the program
		}

		/*
			This is block of code will check if the user wants to
			use the showpid fucnction.
			If not, the program will assume the user wants to exeicute
			other commands.
		*/
		if (strncmp( command, "showpid" , 7) == 0){
			showpid(pidList);
		} else if (strncmp( command, "cd" , 2) == 0){ // cd is a special command. If the child process gets
													  // the cd command, the child process will go into the directory 
													  // and die there. This command will change where the parent process is.
			chdir(opt1);
		} else{
			pass_input(command, opt1, opt2, opt3, opt4, opt5, pidList);
		}

		/*
			I reset all of the options back to NULL so that
			the execl function will stop where the options end
		*/
		opt1 = NULL;
		opt2 = NULL;
		opt3 = NULL;
		opt4 = NULL;
		opt5 = NULL;

	}

	return 0;
}