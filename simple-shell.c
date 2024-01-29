#include "simple-shell.h"
#include <stdio.h>
#include <stdlib.h>
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

/*
 *Returns the enum value of the command.
 *@param command The command to check.
 *@return The enum value of the command.
 */
builtins get_enum (char * command) {
    if (strcmp(command, "cd") == 0) return CD;
    if (strcmp(command, "history") == 0) return HISTORY;
    if (strcmp(command, "alias") == 0) return ALIAS;
    if (strcmp(command, "unalias") == 0) return UNALIAS;
    if (strcmp(command, "getpath") == 0) return GETPATH;
    if (strcmp(command, "setpath") == 0) return SETPATH;
    if (strcmp(command, "exit") == 0) return EXIT;
    return NONE;
}

Command *load_history(int *history_index){
    
    FILE *historyptr;
    Command *history = malloc(sizeof(Command) * MAX_HISTORY);
    historyptr = fopen(".hist_list", "r");
    char *nextLine = malloc(sizeof(char) * 512);
    char **tokens = malloc(sizeof(char**)*MAX_TOKENS);
    int number_of_tokens = 0;
    if(historyptr == NULL){
        perror("Couldn't find file");
        return history;
    }
    while(fgets(nextLine,MAX_INPUT_LENGTH,historyptr) != NULL){
        //nextLine[strcspn(nextLine, "\n")] = 0;
        number_of_tokens = tokenize(nextLine, tokens);
        add_to_history(tokens, history, history_index);
        for (int i = 0; i < number_of_tokens; i++){
            memset(tokens[i], 0, strlen(tokens[i]));
        }
    }
    free(nextLine);
    for (int i = 0; i < number_of_tokens; i++){
            free(tokens[i]);
    }
    free(tokens);
    fclose(historyptr);
    return history;
}

void save_history(Command *history, int *history_index){
    FILE *historyptr;
    historyptr = fopen(".hist_list","w");
    for(int i = 0; i<MAX_HISTORY; i++){
        if(strcmp(history[*history_index].line,"\0")){
            fprintf(historyptr, "%s\n" ,history[*history_index].line);
        }
        (*history_index) = ((*history_index) + 1) % MAX_HISTORY; // wrap around when reaching MAX_HISTORY
    }
    fclose(historyptr);
}

void add_to_history(char **command, Command *history, int *history_index) {
    char temp[MAX_INPUT_LENGTH] = "";
    int i =0;
    while(command[i] != NULL){
        strcat(temp, " ");
        strcat(temp, command[i]);
        i++;
    }
    strcpy(history[(*history_index) % MAX_HISTORY].line, temp);
    strcpy(temp, " ");
    history[(*history_index) % MAX_HISTORY].number = *history_index + 1; 
    (*history_index) = ((*history_index) + 1) % MAX_HISTORY; // wrap around when reaching MAX_HISTORY
}

void print_history(Command *history, int history_index) {
    for (int i = 0; i < MAX_HISTORY; i++) {
        int index = (history_index + i) % MAX_HISTORY; 
        if (history[index].number != 0) { 
            printf("%d %s\n", i + 1, history[index].line);
        }
    }
}