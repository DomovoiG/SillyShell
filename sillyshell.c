/***************************************************
* Author: Austin Gallup
* Date: 2/3/22
* Class: CS-333
* Program: sillyshell.c, mimmics a shell.
***************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

//====================CONSTANTS=====================
int const MAX_USER_INPUT = 100;

//====================GLOABALS=====================
int flag = 0;
int background_flag = 0;


/*==================================================
*init_shell
*DESC: displays "initializing sequence" to user.
*RETURN: void
*ARGS: none
*==================================================*/
void
init_shell()
{
	printf("\n*********************************\n");
	printf("*____SILLYSHELL__INITIALIZING___*\n");
	printf("*********************************\n");
	return;
}

/*==================================================
*shell_prompt
*DESC: Displays "prompt" on command line
*RETURN: void
*ARGS: none
*==================================================*/
void
shell_prompt() 
{
	printf("SILLY>>");
}

/*==================================================
*userin
*DESC: Takes user input as string, and modifies argument
*string (str) to match user input.l
*RETURN: int, 0 for sucess. 1 for failure.
*ARGS: char* str
*==================================================*/
char*
userin() 
{
	char input_buffer[MAX_USER_INPUT];
	scanf(" %[^\n]", input_buffer);
	if(strlen(input_buffer) != 0)
	{
		//malloc
		char* str = (char*)malloc(strlen(input_buffer) * sizeof(char));
		strcpy(str, input_buffer);
		printf("%s\n", str);
		return str; //success!
	}
	else
	{
		flag = 1; 
		return NULL; //failed
	}
	
}


/*==================================================
*tokenize
*DESC: Takes string (input) and tokenizes it modifying
*string argument (output) to hold each token as a string.
*RETURN: void
*ARGS: char *input_string, char** output 
*==================================================*/
char**
tokenize(char* input)
{
	int num_args = 0;
	for(int i = 0; i++; i < strlen(input))
	{
		if(&input[i] ==  " ")
			num_args++;
	}

	char** output = (char**)malloc(num_args *sizeof(char));
	char* token = strtok(input, " ");
	int j = num_args;
	num_args = 0;
	while(token != NULL)
	{
		if(!strcmp(token, "&"))
		{
			background_flag = 1;
			token = strtok(NULL, " ");
		}
		else
		{ 
			output[num_args] = (char*)malloc(strlen(token) * sizeof(char));
			strcpy(output[num_args++], token);
			token = strtok(NULL, " ");
		}
			
	}

	//free all mem
	free(token);
	return output;
}

/*==================================================
*run_args
*DESC: runs the command line commands found in
*tokenized list "arg_list"
*RETURN: void
*ARGS: char **arg_list
*==================================================*/
void
run_args(char** arg_list)
{
	if((!strcmp(arg_list[0], "exit") || !strcmp(arg_list[0], "quit")))
	{
		flag = 1;
		return;//normal exit
	}
	
	pid_t pid = fork();

	if(pid == -1)
	{
		printf("\n failed forking child..\n");
		return;//failed fork 
	}
	else if(pid == 0)
	{
		//in child
		if(!(strcmp(arg_list[0], "sleeper-3")))
		{
			char tmp[13] = "./sleeper-3\0";
			strcpy(arg_list[0], tmp);
		}
		if(execvp(arg_list[0], arg_list) < 0)
		{
			printf("\ncommand unable to be executed...\n");
		}
		exit(0);
	}
	else
	{
		//in parent
		if(background_flag == 0)//if the child process is running in foreground.
			wait(0);
		return;
	}
	return;
}


/*==================================================
*sigchld_handler
*DESC: Sets up signal handler
*RETURN: void
*ARGS: int signum
*==================================================*/
void
sigchld_handler(int signum)
{
	int status;
	pid_t pid;

	while((pid = waitpid(-1, &status, WNOHANG)) > 0) {}
}

/*==================================================
*sigchld_start
*DESC: starts handler
*RETURN: void
*ARGS: none
*==================================================*/
void
sigchld_start()
{
        struct sigaction new_action, old_action;

        new_action.sa_handler = sigchld_handler;
        sigemptyset (&new_action.sa_mask);
        new_action.sa_flags = 0;

        sigaction(SIGCHLD, NULL, &old_action);
        if (old_action.sa_handler != SIG_IGN)
                sigaction(SIGCHLD, &new_action, NULL);

}


/*==================================================
*shell
*DESC: Runs the shell.
*RETURN: void
*ARGS: none
*==================================================*/
void
shell()
{

	init_shell();
	char* input_string;
	char** arg_list;
	
	if(signal(SIGCHLD, sigchld_handler) == SIG_ERR)
	{
		printf("ERROR signal failed\n");
		exit(1);
	}
	do {
		background_flag = 0;		
		shell_prompt();
		input_string = userin(flag);
		arg_list = tokenize(input_string);
		run_args(arg_list);
		free(input_string);	


		if(flag == 1) {
			printf("\nClosing Shell...\n");
			exit(0);
		}

	}while(1);
	
	int i = 0;
	while(arg_list != NULL)
	{
		free(arg_list[i++]);
	}
	free(arg_list);
	return;
}

/*=======================MAIN===========================*/

int
main()
{
	sigchld_start();
	shell();
	return 0;
}
