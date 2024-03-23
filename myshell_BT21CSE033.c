/********************************************************************************************
This is a template for assignment on writing a custom Shell. 

Students may change the return types and arguments of the functions given in this template,
but do not change the names of these functions.

Though use of any extra functions is not recommended, students may use new functions if they need to, 
but that should not make code unnecessorily complex to read.

Students should keep names of declared variable (and any new functions) self explanatory,
and add proper comments for every logical step.

Students need to be careful while forking a new process (no unnecessory process creations) 
or while inserting the single handler code (should be added at the correct places).

Finally, keep your filename as myshell.c, do not change this name (not even myshell.cpp, 
as you not need to use any features for this assignment that are supported by C++ but not by C).
*********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>			// exit()
#include <unistd.h>			// fork(), getpid(), exec()
#include <sys/wait.h>		// wait()
#include <signal.h>			// signal()
#include <fcntl.h>			// close(), open()

int cur_size=0;
pid_t PID;
void sighandler(int signum)
{
	// printf("You pressed ctrl C\n");
	if (PID != 0)
	{
		kill(PID, SIGKILL);
	}
	// exit(1);
}
// void ctrlZhandler(int signum)
// {
// 	printf("You pressed ctrl Z");
// }

int parseInput(char ** commands,char * delimiter, char * str )
{
	// This function will parse the input string into multiple commands
	// or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).

	int i = 0;
    char *token;
    char *rest = str;
	token = strsep(&rest, delimiter);
    while (token!= NULL) {
        if (*token != '\0') { // Check if the token is not empty
            commands[i++] = token;
        }
		token = strsep(&rest, delimiter);
    }

    commands[i] = NULL;
    return i;
}

void printInput(char** commands)
{
	for(int i=0;commands[i]!=NULL;i++ )
	{
		if(commands[i]==NULL)
		{
			printf("NULL\n");
		}
		printf("%s \n",commands[i]);
	}
}

void printError()
{
	printf("Shell: Incorrect command\n");
}

int parseAssert(char * args,char * delimiter )
{
	if(strstr(args,delimiter)!=NULL)
	{
		return 1;
	}else{
		return 0;
	}
}

void executeCommand(char ** args)
{
	if(strcmp(args[0],"cd")==0)
		{
			int result;
			if(args[1]==NULL)
			{
				result=chdir(getenv("HOME"));
			}
			else
			{
				result=chdir(args[1]);
			}

			if(result!=0 ) //&& strcmp(args[1],"..")!=0
			{
				printf("Shell: Incorrect command\n");
			}
		}
	else{	
	// This function will fork a new process to execute a command
	PID=fork();


	if(PID==0)
	{	
		if(strcmp(args[0],"cd")!=0 && strcmp(args[0],"exit")!=0)
		{
		execvp(args[0],args);
		printError();
		exit(0);
		}
	}else{
		wait(NULL);
	}
	}
}

void commandRedirection(char **str)
{
	int i = 0;
	char *commands[1000];
	for (int k = 0; k < cur_size; k++)
	{
		char *original = strdup(str[k]);

		while (i < 1000)
		{
			commands[i] = strsep(&original, " ");
			if (commands[i] == NULL)
				break;
			if (strlen(commands[i]) != 0)
				i++;
		}
	}

	int n = i;

	PID = fork();

	if (PID < 0)
	{
		printf("Shell: Incorrect command\n");
		exit(0);
	}
	else if (PID == 0)
	{
		close(STDOUT_FILENO); // Redirecting STDOUT
		open(commands[n - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644 );
		// open(commands[n - 1], O_CREAT | O_RDWR | O_APPEND);

		commands[n - 1] = NULL; // making name null as it is not required
		execvp(commands[0], commands);

		printf("Shell: Incorrect command\n");
		exit(0);
	}
	else
	{
		wait(NULL);
	}
}

void executeCommandRedirection(char * delivery_file,char ** args,char * input)
{
	// This function will run a single command with output redirected to an output file specificed by user
	if(delivery_file!=NULL)
	{	PID = fork();
		if(PID==0)
		{
			// int fd= open(delivery_file, O_WRONLY|O_CREAT|O_TRUNC,0644);

			// dup2(fd, STDOUT_FILENO);
			// close(fd);
			//-------------------------------------
			close(STDOUT_FILENO); // Redirecting STDOUT
			open(delivery_file, O_CREAT | O_RDWR | O_APPEND);

			delivery_file = NULL; 
			//-------------------------------------
			execvp(args[0],args);
			printError();
			exit(0);
		}
		else
		{
			wait(NULL);
		}
	}
	// else
	// {
	// 	//-----------------------------------------------------------------------------------
	// 	int cur_size=0;char * commands1[200];
	// 	cur_size= parseInput(commands1,">",input);
	// 	//----------------------------------------------------------------------------------
	// 	int i = 0;
	// 	char *commands[300];
	// 	for (int k = 0; k < cur_size; k++)
	// 	{
	// 		char *original = strdup(commands1[k]);

	// 		while (i < 300)
	// 		{
	// 			commands[i] = strsep(&original, " ");
	// 			if (commands[i] == NULL)
	// 				break;
	// 			// if (strlen(commands[i]) != 0)
	// 			// 	i++;
	// 		}
	// 	}
	// 	//-----------------------------------------------------------------------------------
	// 	int n = i;

	// 	PID = fork();

	// 	if (PID < 0)
	// 	{
	// 		printf("Shell: Incorrect command\n");
	// 		exit(0);
	// 	}
	// 	else if (PID == 0)
	// 	{
	// 		close(STDOUT_FILENO); // Redirecting STDOUT
	// 		open(commands[n - 1], O_CREAT | O_RDWR | O_APPEND);

	// 		commands[n - 1] = NULL; // making name null as it is not required
	// 		execvp(commands[0], commands);

	// 		printf("Shell: Incorrect command\n");
	// 		exit(0);
	// 	}
	// 	else
	// 	{
	// 		wait(NULL);
	// 	}
	// 	//-----------------------------------------------------------------------------------
	// }
}

void executeParallelCommands(char ** commands, char * input,int size)
{
	// This function will run multiple commands in parallel
	int numberOfCommands=size;

	// while(commands[numberOfCommands]!=NULL)
	// {
	// 	numberOfCommands++;
	// }

	// pid_t childPids[numberOfCommands];

	for(int i =0;i<numberOfCommands; i++)
	{
		char* args[1000];
		char* delivery_file = NULL;

		if(parseAssert(commands[i],">")==1)
		{
			int max_idx=parseInput(args,">",commands[i]);
			delivery_file=args[max_idx-1];
			parseInput(args," ",args[0]);
			executeCommandRedirection(delivery_file,args,input);
		}
		else
		{
			parseInput(args," ",commands[i]);
			PID = fork();

			if(PID==0)
			{
				executeCommand(args);
				printError();
				exit(0);
			}
			// else
			// {
			// 	childPids[i]=PID;
			// }

		}
	}
	for(int i=0;i<numberOfCommands;i++)
	{
		// waitpid(childPids[i],NULL,0);
		wait(NULL);
	}

}

void executeSequentialCommands(char** commands,char* input)
{	
	// This function will run multiple commands in parallel
	int i=0;
	while(commands[i])
	{
		char * args[100];
		char * delivery_file = NULL;
		if(parseAssert(commands[i],">")==1)
		{
			int max_idx=parseInput(args,">",commands[i]);
			delivery_file = args[max_idx-1];
			parseInput(args," ",commands[0]);
			executeCommandRedirection(delivery_file,args,input);
		}
		else
		{
			parseInput(args," ",commands[i]);
			executeCommand(args);
		}
		i++;
	}
}

// void executePipes(char * str)
// {

// 	char * arggs[100];
// 	int command_num=parseInput(arggs,"|",str);
// 	int fd[command_num][2];
// 	for (int i=0;i<command_num;i++)
// 	{
// 		pipe(fd[i]);
// 	}

// 	for(int i=0;i<command_num-1;i+=2)
// 	{
// 		char * commands[100];
// 		int pid1=fork();
// 		if(pid1==0)
// 		{
// 			dup2(fd[i][1],STDOUT_FILENO);
// 			close(fd[i][1]);
// 			for(int j=0;j<command_num;j++)
// 			{
// 				close(fd[j][0]);
// 				close(fd[j][1]);
// 			}
// 			parseInput(commands," ", arggs[i]);
// 			executeCommand(commands);
// 		}
// 		int pid2=fork();
// 		if(pid2==0)
// 		{
// 			dup2(fd[i][0],STDIN_FILENO);
// 			close(fd[i][0]);
// 			for(int j=0;j<command_num;j++)
// 			{
// 				close(fd[j][0]);
// 				close(fd[j][1]);
// 			}
// 			parseInput(commands," ", arggs[i+1]);
// 			executeCommand(commands);
// 		}
// 		waitpid(pid1,NULL,0);
// 		waitpid(pid2,NULL,0);
// 	}

// }
void executePipes(char *str) {
    char *args[100];
    int command_num = parseInput(args, "|", str);
    int fd[command_num - 1][2]; 

    for (int i = 0; i < command_num - 1; i++) {
        pipe(fd[i]);
    }

    for (int i = 0; i < command_num; i++) {
        char *commands[100];
        parseInput(commands, " ", args[i]);
        int pid = fork();

        if (pid == 0) {
            
            if (i > 0) {
            
                dup2(fd[i - 1][0], STDIN_FILENO);
                close(fd[i - 1][0]);
            }
            if (i < command_num - 1) {
            
                dup2(fd[i][1], STDOUT_FILENO);
                close(fd[i][1]);
            }
            
            for (int j = 0; j < command_num - 1; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }
            executeCommand(commands); 
            exit(0);
        }

            if (i > 0) {
            close(fd[i - 1][0]);
            close(fd[i - 1][1]);
        }
    }

    
    for (int i = 0; i < command_num - 1; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    
    for (int i = 0; i < command_num; i++) {
        wait(NULL);
    }
}


int main()
{
	signal(SIGINT, sighandler);
	signal(SIGTSTP, sighandler);

	// Initial declarations
	char currentWorkingDirectory_buffer[1000];
	char * currentWorkingDirectory;
	size_t lineSize=0;
	char * buffer=NULL;
	char ** arguments=NULL;
	char * delimiter1=" ";

	// printf("print is %s and lineSize is %ld",buff,lineSize);
		// if(signal(SIGINT,ctrlChandler))
			// goto restart;
			// signal(SIGINT,ctrlChandler);
	
	while(1)	// This loop will keep your shell running until user exits.
	{
		char *commands[100];
		// Print the prompt in format - currentWorkingDirectory$
		currentWorkingDirectory=getcwd(currentWorkingDirectory_buffer,sizeof(currentWorkingDirectory_buffer));
		printf("%s$",currentWorkingDirectory);
		// accept input with 'getline()'

	restart:
		getline(&buffer,&lineSize,stdin);

		// printf("%d",strlen(buffer));

		buffer[strlen(buffer)-1]='\0';


		// printf("%d",strlen(buffer));
		// printf("%s",buffer);

		char* src=malloc(sizeof(char)*strlen(buffer));
		char* str=malloc(sizeof(char)*strlen(buffer));
		
		strcpy(src,buffer);
		strcpy(str,buffer);

		// int n=parseAssert(str,"&&");
		// printf("value of n is %d",n);
		
		char* token=strtok(src," ");
		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		// arguments=parseInput(buffer,delimiter1); 		
		
		if(strcmp(token,"exit")==0)	// When user uses exit command.
		{
			printf("Exiting shell...\n");
			break;
		}
// ------------------------------------------------------------
		// int j=0;
		// char* cdCommands[1000];

		// 	while(token!=NULL)
		// {
		// 	cdCommands[j++]=token;
		// 	token=strtok(NULL," ");
		// 	// printf("%s\n", commands[i-1]);
		// }
		// cdCommands[j]=NULL;

		// if(strcmp(cdCommands[0],"cd")==0)
		// {
		// 	if(chdir(cdCommands[1])!=0)
		// 	{
		// 		perror("Shell: Incorrect command");
		// 	}
		// }
// ------------------------------------------------------------
		if(parseAssert(str,"|")==1){
			char * str_copy=malloc(sizeof(char*)*strlen(str));
			strcpy(str_copy,str);
			int max_idx=parseInput(commands," ",str_copy);
			int i=0;
			while(commands[max_idx-1][i]!='\0')
			{
				i++;
			}
			if(commands[max_idx-1][i-1]=='|')
			{
				printError();
				continue;
			}
			else
			{
				executePipes(str);
			}

			// int z=parseInput(arggs," ",commands[max_idx]);
			// // printInput(arggs);
			// if(arggs[z-1]==NULL)
			// {
			// 	printError();
			// 	break;
			// }

		}
		else if(parseAssert(str,"&&")==1){
			int size=parseInput(commands,"&&",str);
			executeParallelCommands(commands,str,size);		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
			// printf("working ....\n");

		}
		else if(parseAssert(str,"##")==1 ){ //(|| parseAssert(str,">")==1)
			// printf("working...\n");
			parseInput(commands,"##",str);
			executeSequentialCommands(commands,str);	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		}
		else if(parseAssert(str,">")==1)
		{
			int i=0;
			for (i = 0; i < 100; i++)
			{
				commands[i] = strsep(&str, ">");

				if (commands[i] == NULL)
					break;
				// if (strlen(commands[i]) == 0)
				// 	i--;
			}
			cur_size = i;

			commandRedirection(commands);

		}
	// 	else if(parseAssert(str,">")==1){
	// 		// printf("working...\n");
	// 		parseInput(commands,"##",str);
	// 		executeSequentialCommands(commands);
	// 		// printInput(commands);
	// //		executeCommandRedirection(commands[1],commands);	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
	// 	}
		else{
			parseInput(commands," ",str);
			executeCommand(commands);		// This function is invoked when user wants to run a single commands
		}
				
	}
	
	return 0;
}