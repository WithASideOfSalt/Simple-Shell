#include "simple-shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Used for getcwd
#include <sys/types.h>
#include <sys/wait.h>

int forky_fun(char *command, char* arguments[], int args_len){
    pid_t parent = getpid();
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0); // Wait for the child to finish
        if (status == 0){
            return 0;
        } else if (status==256) {
            printf("Error, command not found\n");
            return -1;
        } 
        else {
            printf("Status:%d\n", status);
            return -1;
        }
        
    } else { // This is the child proccess
        char **argv; // Pointer for the argument array
        argv = malloc(sizeof(char *) * args_len+2);
    
        argv[0] = malloc(strlen(command));
        strcpy(argv[0], command);

        int i = 0;
        while (i<args_len){
            argv[i+1] = malloc(strlen(arguments[i]));
            strcpy(argv[i+1], arguments[i]);
            i++;
        }
        argv[i+1] = malloc(sizeof(NULL));
        argv[i+1] = NULL;
        execvp(command, argv);
        _exit(EXIT_FAILURE);
    }
}

int tokenize(char *str, char **tokens){
    /*
    Return the number of tokens.
    Tokens are put in tokens.
    */
    char *token; 
    int i = 0;
    token = strtok(str, DELIMITERS);
    while (token != NULL){
        tokens[i] = malloc(strlen(token));
        strcpy(tokens[i], token);
        token = strtok(NULL, DELIMITERS);
        i++;
    }
    return i;
}