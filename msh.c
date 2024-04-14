// P2-SSOO-23/24

//  MSH main file
// Write your msh source code here

// #include "parser.h"
#include <stddef.h> /* NULL */
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

// to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
    printf("****  Exiting MSH **** \n");
    // signal(SIGINT, siginthandler);
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
struct command *history;
int head = 0;
int tail = 0;
int n_elem = 0;

void free_command(struct command *cmd)
{
    if ((*cmd).argvv != NULL)
    {
        char **argv;
        for (; (*cmd).argvv && *(*cmd).argvv; (*cmd).argvv++)
        {
            for (argv = *(*cmd).argvv; argv && *argv; argv++)
            {
                if (*argv)
                {
                    free(*argv);
                    *argv = NULL;
                }
            }
        }
    }
    free((*cmd).args);
}

void store_command(char ***argvv, char filev[3][64], int in_background, struct command *cmd)
{
    int num_commands = 0;
    while (argvv[num_commands] != NULL)
    {
        num_commands++;
    }

    for (int f = 0; f < 3; f++)
    {
        if (strcmp(filev[f], "0") != 0)
        {
            strcpy((*cmd).filev[f], filev[f]);
        }
        else
        {
            strcpy((*cmd).filev[f], "0");
        }
    }

    (*cmd).in_background = in_background;
    (*cmd).num_commands = num_commands - 1;
    (*cmd).argvv = (char ***)calloc((num_commands), sizeof(char **));
    (*cmd).args = (int *)calloc(num_commands, sizeof(int));

    for (int i = 0; i < num_commands; i++)
    {
        int args = 0;
        while (argvv[i][args] != NULL)
        {
            args++;
        }
        (*cmd).args[i] = args;
        (*cmd).argvv[i] = (char **)calloc((args + 1), sizeof(char *));
        int j;
        for (j = 0; j < args; j++)
        {
            (*cmd).argvv[i][j] = (char *)calloc(strlen(argvv[i][j]), sizeof(char));
            strcpy((*cmd).argvv[i][j], argvv[i][j]);
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
void getCompleteCommand(char ***argvv, int num_command)
{
    // reset first
    for (int j = 0; j < 8; j++)
        argv_execvp[j] = NULL;

    int i = 0;
    for (i = 0; argvv[num_command][i] != NULL; i++)
        argv_execvp[i] = argvv[num_command][i];
}

/**
 * Main sheell  Loop
 */
int main(int argc, char *argv[])
{
    /**** Do not delete this code.****/
    int end = 0;
    int executed_cmd_lines = -1;
    char *cmd_line = NULL;
    char *cmd_lines[10];

    if (!isatty(STDIN_FILENO))
    {
        cmd_line = (char *)malloc(100);
        while (scanf(" %[^\n]", cmd_line) != EOF)
        {
            if (strlen(cmd_line) <= 0)
                return 0;
            cmd_lines[end] = (char *)malloc(strlen(cmd_line) + 1);
            strcpy(cmd_lines[end], cmd_line);
            end++;
            fflush(stdin);
            fflush(stdout);
        }
    }

    /*********************************/

    char ***argvv = NULL;
    int num_commands;

    history = (struct command *)malloc(history_size * sizeof(struct command));
    int run_history = 0;

    while (1)
    {
        int status = 0;
        int command_counter = 0;
        int in_background = 0;
        signal(SIGINT, siginthandler);

        if (run_history)
        {
            run_history = 0;
        }
        else
        {
            // Prompt
            write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

            // Get command
            //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
            executed_cmd_lines++;
            if (end != 0 && executed_cmd_lines < end)
            {
                command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
            }
            else if (end != 0 && executed_cmd_lines == end)
                return 0;
            else
                command_counter = read_command(&argvv, filev, &in_background); // NORMAL MODE
        }
        //************************************************************************************************

        /************************ STUDENTS CODE ********************************/
        static int internal_accumulator = 0;
        // error handler
        if (command_counter > 0)
        {
            if (command_counter > MAX_COMMANDS)
            {
                printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
            }
            if (command_counter > MAX_EXECUTABLE_COMMANDS)
            {
                perror("Maximum number of executable commands is 3");
            }
        }

        for (int i = 0; i < command_counter; i++)
        {
            getCompleteCommand(argvv, i);
        }

        if (strcmp(argv_execvp[0], "mycalc") == 0)
        {
            if (argv_execvp[1] == NULL || argv_execvp[2] == NULL || argv_execvp[3] == NULL)
            {
                printf("[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");
            }
            else
            {
                int num1, num2, result;
                char *endptr;
                char *operator= argv_execvp[2];
                num1 = atoi(argv_execvp[1]);
                num2 = atoi(argv_execvp[3]);
                result = 0;
                if (strcmp(operator, "add") == 0)
                {
                    result = num1 + num2;
                    internal_accumulator = internal_accumulator + result;
                    printf("[OK] %d + %d = %d; Acc %d\n", num1, num2, result, internal_accumulator);
                }
                else if (strcmp(operator, "mul") == 0)
                {
                    result = num1 * num2;
                    printf("[OK] %d * %d = %d\n", num1, num2, result);
                }
                else if (strcmp(operator, "div") == 0)
                {
                    result = num1 / num2;
                    int remainder = num1 % num2;
                    printf("[OK] %d / %d = %d; Remainder %d\n", num1, num2, result, remainder);
                }
            }
        }
        else if (command_counter == 1)
        {
            int pid = fork();
            if (pid == -1)
            {
                perror("Error en fork: Reescribir");
                return (-1);
            }

            int filehandle = 0;
            int stat;
            // Hijo
            if (pid == 0)
            {
                // Redirecciones de salida, entrada y error
                if (strcmp(filev[1], "0") != 0)
                {

                    if ((close(1)) < 0)
                    {
                        perror("Error al cerrar descriptor");
                    }

                    if ((filehandle = open(filev[1], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0)
                    {
                        perror("Error al abrir fichero\n");
                    }
                }
                if (strcmp(filev[0], "0") != 0)
                {
                    if ((close(0)) < 0)
                    {
                        perror("Error al cerrar descriptor");
                    }
                    if ((filehandle = open(filev[0], O_RDWR, 0644)) < 0)
                    {
                        perror("Error al abrir fichero\n");
                    }
                }
                if (strcmp(filev[2], "0") != 0)
                {
                    if ((close(2)) < 0)
                    {
                        perror("Error al cerrar descriptor");
                    }
                    if ((filehandle = open(filev[2], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0)
                    {
                        perror("Error al abrir fichero\n");
                    }
                }
                // Hacemos que el hijo ejecute

                if (execvp(argv_execvp[0], argv_execvp) < 0)
                {
                    perror("Error al ejecutar\n");
                }
            }
            else
            { // padre
                if (filehandle != 0)
                {
                    if ((close(filehandle)) < 0)
                    {
                        perror("Error al cerrar descriptor");
                    }
                }
                if (!in_background)
                {
                    while (wait(&stat) > 0)
                        ;
                    if (stat < 0)
                    {
                        perror("Error ejecucion hijo\n"); // Cambiar todos los errores por perror
                    }
                }
            }
        }
        else
        {
            int n = command_counter;
            int fd[2];
            int pid, status2;
            int filehandle = 0;

            int in;

            if ((in = dup(0)) < 0)
            {
                perror("Error al duplicar descriptor\n");
            }

            for (int i = 0; i < n; i++)
            {
                // Creacion del siguiente pipe, si es el ultimo proceso, no se crea
                if (i != n - 1)
                {
                    if (pipe(fd) < 0)
                    {
                        perror("Error en pipe\n");
                        exit(0);
                    }
                }

                /* se crea el proceso siguiente en la cadena */
                switch (pid = fork())
                {

                case -1:
                    perror("Error en fork\n");

                    if ((close(fd[0])) < 0)
                    {
                        perror("Error al cerrar descriptor");
                    }
                    if ((close(fd[1])) < 0)
                    {
                        perror("Error al cerrar descriptor");
                    }
                    exit(0);
                    // Proceso hijo
                case 0:
                    // La entrada estandar pasa a ser la del hijo anterior, que viene dado por in

                    // En procesos intermedios, la redireccion de error es valida
                    if (strcmp(filev[2], "0") != 0)
                    {
                        if ((close(2)) < 0)
                        {
                            perror("Error al cerrar descriptor");
                        }

                        if ((filehandle = open(filev[2], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0)
                        {
                            perror("Error al abrir fichero\n");
                        }
                    }

                    if (i == 0 && strcmp(filev[0], "0") != 0)
                    {
                        if ((close(0)) < 0)
                        {
                            perror("Error al cerrar descriptor");
                        }
                        if ((filehandle = open(filev[0], O_RDWR, 0644)) < 0)
                        {
                            perror("Error al abrir fichero\n");
                        }
                    }
                    else
                    {
                        if ((close(0)) < 0)
                        {
                            perror("Error al cerrar descriptor");
                        }
                        if (dup(in) < 0)
                        {
                            perror("Error al duplicar descriptor\n");
                        }
                        if ((close(in)) < 0)
                        {
                            perror("Error al cerrar descriptor");
                        }
                    }

                    // Si no es el ultimo proceso, cierra la salida estandar
                    if (i != n - 1)
                    {

                        if ((close(1)) < 0)
                        {
                            perror("Error al cerrar descriptor");
                        }

                        if (dup(fd[1]) < 0)
                        {
                            perror("Error al duplicar descriptor\n");
                        }
                        if ((close(fd[0])) < 0)
                        {
                            perror("Error al cerrar descriptor");
                        }
                        if ((close(fd[1])) < 0)
                        {
                            perror("Error al cerrar descriptor");
                        }
                    }
                    else
                    {
                        if (strcmp(filev[1], "0") != 0)
                        {
                            if ((close(1)) < 0)
                            {
                                perror("Error al cerrar descriptor");
                            }

                            if ((filehandle = open(filev[1], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0)
                            {
                                perror("Error al abrir fichero\n");
                            }
                        }
                    }

                    getCompleteCommand(argvv, i);
                    if (in_background)
                    {
                        printf("[%d]\n", getpid());
                    }

                    if (execvp(argv_execvp[0], argv_execvp) < 0)
                    {
                        perror("Error al ejecutar\n");
                    }
                    break;

                    // proceso padre
                default:
                    // El padre le da el nuevo valor a in para que pueda utilizarlo el hijo a no ser que sea el ultimo proceso

                    if ((close(in)) < 0)
                    {
                        perror("Error al cerrar descriptor");
                    }
                    if (i != n - 1)
                    {
                        if ((in = dup(fd[0])) < 0)
                        {
                            perror("Error al duplicar descriptor\n");
                        }
                        if (dup(fd[0]) < 0)
                        {
                            perror("Error al duplicar descriptor\n");
                        }
                        if ((close(fd[1])) < 0)
                        {
                            perror("Error al cerrar descriptor");
                        }
                    }
                }
            }
            if (filehandle != 0)
            {

                if ((close(filehandle)) < 0)
                {
                    perror("Error al cerrar descriptor");
                }
            }
            // Al terminar el bucle, el primer proceso espera al último, que irá despertando a todos
            if (!in_background)
            {
                while (wait(&status2) > 0)
                    ;
                if (stat < 0)
                {
                    perror("Error ejecucion hijo\n"); // Cambiar todos los errores por perror
                }
            }
        }
    }
    return 0;
}
