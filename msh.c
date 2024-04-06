//P2-SSOO-23/24

//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMANDS 8
#define MAX_EXECUTABLE_COMMANDS 3


// files in case of redirection
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Exiting MSH **** \n");
	//signal(SIGINT, siginthandler);
	exit(0);
}

/* myhistory */

/* myhistory */

struct command
{
  // Store the number of commands in argvv
  int num_commands;
  // Store the number of arguments of each command
  int *args;
  // Store the commands
  char ***argvv;
  // Store the I/O redirection
  char filev[3][64];
  // Store if the command is executed in background or foreground
  int in_background;
};

int history_size = 20;
struct command * history;
int head = 0;
int tail = 0;
int n_elem = 0;

void free_command(struct command *cmd)
{
    if((*cmd).argvv != NULL)
    {
        char **argv;
        for (; (*cmd).argvv && *(*cmd).argvv; (*cmd).argvv++)
        {
            for (argv = *(*cmd).argvv; argv && *argv; argv++)
            {
                if(*argv){
                    free(*argv);
                    *argv = NULL;
                }
            }
        }
    }
    free((*cmd).args);
}

void store_command(char ***argvv, char filev[3][64], int in_background, struct command* cmd)
{
    int num_commands = 0;
    while(argvv[num_commands] != NULL){
        num_commands++;
    }

    for(int f=0;f < 3; f++)
    {
        if(strcmp(filev[f], "0") != 0)
        {
            strcpy((*cmd).filev[f], filev[f]);
        }
        else{
            strcpy((*cmd).filev[f], "0");
        }
    }

    (*cmd).in_background = in_background;
    (*cmd).num_commands = num_commands-1;
    (*cmd).argvv = (char ***) calloc((num_commands) ,sizeof(char **));
    (*cmd).args = (int*) calloc(num_commands , sizeof(int));

    for( int i = 0; i < num_commands; i++)
    {
        int args= 0;
        while( argvv[i][args] != NULL ){
            args++;
        }
        (*cmd).args[i] = args;
        (*cmd).argvv[i] = (char **) calloc((args+1) ,sizeof(char *));
        int j;
        for (j=0; j<args; j++)
        {
            (*cmd).argvv[i][j] = (char *)calloc(strlen(argvv[i][j]),sizeof(char));
            strcpy((*cmd).argvv[i][j], argvv[i][j] );
        }
    }
}


/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
	//reset first
	for(int j = 0; j < 8; j++)
		argv_execvp[j] = NULL;

	int i = 0;
	for ( i = 0; argvv[num_command][i] != NULL; i++)
		argv_execvp[i] = argvv[num_command][i];
}

/**
 * Main sheell  Loop
 */
int main(int argc, char *argv[]) {

    history = (struct command *)calloc(history_size, sizeof(struct command));
    if (history == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for history\n");
        return 1; // Return an error code indicating failure
    }

    char ***argvv = NULL;
    char *filev[3];
    int bg = 0;

    while (1) {
        // Read command
        int command_counter = read_command(&argvv, filev, &bg);

        if (command_counter > 0) {
            if (command_counter > MAX_COMMANDS) {
                printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
            } else if (command_counter > MAX_EXECUTABLE_COMMANDS) {
                perror("Maximum number of executable commands is 3");
            } else {
                // Execute commands
                for (int i = 0; i < command_counter; i++) {
                    if (strcmp(argvv[i][0], "mycalc") == 0) {
                        if (argvv[i][1] == NULL || argvv[i][2] == NULL || argvv[i][3] == NULL) {
                            fprintf(stderr, "[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");
                        } else {
                            int num1 = atoi(argvv[i][1]);
                            int num2 = atoi(argvv[i][3]);
                            char *operator = argvv[i][2];
                            int result;
                            if (strcmp(operator, "add") == 0) {
                                result = num1 + num2;
                                printf("[OK] %d + %d = %d\n", num1, num2, result);
                            } else if (strcmp(operator, "mul") == 0) {
                                result = num1 * num2;
                                printf("[OK] %d * %d = %d\n", num1, num2, result);
                            } else if (strcmp(operator, "div") == 0) {
                                if (num2 == 0) {
                                    fprintf(stderr, "[ERROR] Division by zero\n");
                                } else {
                                    result = num1 / num2;
                                    int remainder = num1 % num2;
                                    printf("[OK] %d / %d = %d; Remainder %d\n", num1, num2, result, remainder);
                                }
                            } else {
                                fprintf(stderr, "[ERROR] Invalid operator\n");
                            }
                        }
                    } else if (strcmp(argvv[i][0], "myhistory") == 0) {
                        if (argvv[i][1] == NULL) {
                            // Print last 20 commands
                            int start_index = (history_count > 20) ? (history_count - 20) : 0;
                            for (int j = start_index; j < history_count; j++) {
                                printf("%d %s\n", history[j].cmd_number, history[j].cmd);
                            }
                        } else {
                            // Execute specified command from history
                            int command_number = atoi(argvv[i][1]);
                            if (command_number >= 0 && command_number < history_count) {
                                printf("Running command %d\n", command_number);
                                system(history[command_number].cmd);
                            } else {
                                printf("ERROR: Command not found\n");
                            }
                        }
                    } else {
                        // Handle other commands
                    }
                }
                // Update command history
                for (int i = 0; i < command_counter; i++) {
                    history[history_count].cmd = strdup(argvv[i][0]); // Assuming command name is stored at index 0
                    history[history_count].cmd_number = history_count;
                    history_count = (history_count + 1) % HISTORY_SIZE;
                }
            }
        }

        // Clean up resources
        // Free memory allocated for argvv and filev
        for (int i = 0; i < command_counter; ++i) {
            free_command(&history[i]);
    }

    return 0;
}
