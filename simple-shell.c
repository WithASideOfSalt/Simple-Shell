#include "simple-shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Used for getcwd
#include <sys/types.h>
#include <sys/wait.h>

int history_index = 0;
struct Command history[MAX_HISTORY];

/*
 *Forks and executes an external program.
 *@param command The external process to run.
 *@param arguments An array of arguments for the external process. Note that the first element of this array IS NOT the command, unlike the array passed to the execvp function.
 *@param args_len The amount of arguments for the external process.
 *@return 0 is successful, error code if not.
 */
int forky_fun(char *command, char* arguments[], int args_len){
    pid_t pid = fork();
    if (pid == -1) {
        perror("Failed to create process");
        return -1;
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0); // Wait for the child to finish
        if (status == 0){
            return 0;
        } else if (status==256) { // Program not found error code
            printf("Error: Command not found\n");
            return -1;
        } else if (status==512){ // Argument not found
            return -1;
        } else {
            printf("Status:%d\n", status);
            return -1;
        }
        
    } else { // This is the child proccess
        // Create pointer for the argument array and allocate memory
        char **argv;
        argv = malloc(sizeof(char *) * args_len+2);
        // Put the command in the first position of the argument array, as required by execvp
        argv[0] = malloc(strlen(command));
        strcpy(argv[0], command);
        // Copy all the arguments into the array
        int i = 0;
        while (i<args_len){
            argv[i+1] = malloc(strlen(arguments[i]));
            strcpy(argv[i+1], arguments[i]);
            i++;
        }
        // Finish the array with NULL, as required by execvp
        argv[i+1] = malloc(sizeof(NULL));
        argv[i+1] = NULL;
        // Execute
        execvp(command, argv);
        _exit(EXIT_FAILURE);
    }
}

/*
 *@param str The string to tokenize.
 *@param tokens A double pointer for tokenize to store the formated tokens.
 *@return The number of tokens.
 */
int tokenize(char *str, char **tokens){
    
    char *token; 
    int i = 0;
    token = strtok(str, DELIMITERS);
    while (token != NULL){
        // Allocate memory to store the token
        tokens[i] = malloc(strlen(token));
        // Copy the token
        strcpy(tokens[i], token);
        token = strtok(NULL, DELIMITERS);
        i++;
    }
    return i;
}

builtins get_enum (char * command) {
    if (strcmp(command, "cd") == 0) return CD;
    if (strcmp(command, "history") == 0) return HISTORY;
    if (strcmp(command, "alias") == 0) return ALIAS;
    if (strcmp(command, "unalias") == 0) return UNALIAS;
    if (strcmp(command, "getpath") == 0) return GETPATH;
    if (strcmp(command, "setpath") == 0) return SETPATH;
    if (strcmp(command, "!!") == 0) return LAST_COMMAND;
    if (strcmp(command, "exit") == 0) return EXIT;
    return 0;
}

void add_to_history(char *Command) {
    strcpy(history[history_index % MAX_HISTORY].line, Command);
    history[history_index % MAX_HISTORY].number = history_index;
    history_index++;
}

void print_history() {
    int i = 0;
    while (i < MAX_HISTORY) {
        if (history[i].number != 0) {
            printf("%d %s", history[i].number, history[i].line);
        }
        i++;
    }
}