#include "simple-shell.h"
#include <stdio.h>
#include <stdlib.h> //setenv, getenv
#include <string.h>
#include <unistd.h> // Used for getcwd
#include <sys/types.h>
#include <sys/wait.h>

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
        perror("Error:");
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

int get_env(char **tokens, int number_of_tokens) {
    if (number_of_tokens != 1) {
        printf("Error: Incorrect usage of getpath command. Usage: getpath\n");
        return -1;
    }
    char *path_value = getenv("PATH");
 if (path_value != NULL) {
        printf("Current PATH: %s\n", path_value);
    } else {
        printf("Error: PATH not found in the environment.\n");
        return -1;
    }

    return 0;
}

int set_env(char **tokens, int number_of_tokens) {
    if (number_of_tokens != 2) {
        printf("Error: Incorrect usage of setpath command. Usage: setpath <new_path>\n");
        return -1;
    }

    // Set the new path using setenv
    if (setenv("PATH", tokens[1], 1) != 0) {
        perror("setenv() error");
        return -1;
    }

    return 0;
}


int restore_original_path(char *original_path) {
// Restore the PATH to its original value
    if (setenv("PATH", original_path, 1) != 0) {
        perror("setenv() error");
        return -1;
    }

       // Print the restored path
    char *restored_path = getenv("PATH");
    if (restored_path != NULL) {
        printf("Restored PATH: %s\n", restored_path);
    } else {
        fprintf(stderr, "Error: PATH not found in the environment after restoration.\n");
        return -1;
    }

    return 0;
}

int changeDirectory(char **tokens, int number_of_tokens) {
    if (number_of_tokens != 2) {
        printf("Error: Incorrect usage of cd command. Usage: cd <directory>\n");
        return -1;
    }
    else {
        if (chdir(tokens[1]) != 0) {
            perror("chdir() error");
            return -1;
        }
    }
    return 0;
        
}

     

