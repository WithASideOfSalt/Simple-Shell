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
        char error_msg[MAX_INPUT_LENGTH+strlen("Command not found: ")];
        strcpy(error_msg, "Command not found: ");
        strcat(error_msg, command);
        perror(error_msg);
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
        nextLine[strlen(nextLine) -1] = '\0';
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
    if(historyptr == NULL){
        perror("error opening file");
        return;
    }
    for(int i = 0; i<MAX_HISTORY; i++){
        if(strcmp(history[*history_index].line,"")){
            fprintf(historyptr, "%s\n" ,history[*history_index].line);
        }
        (*history_index) = ((*history_index) + 1) % MAX_HISTORY; // wrap around when reaching MAX_HISTORY
    }
    fclose(historyptr);
}

/**
 * Adds a command to the history array.
 *
 * @param command The command to be added to the history.
 * @param history The array of Command structures representing the history.
 * @param history_index A pointer to the current index in the history array.
 */
void add_to_history(char **command, Command *history, int *history_index) {
    // Concatenate the command arguments into a single string
    char temp[MAX_INPUT_LENGTH] = "";
    int i = 0;
    while (command[i] != NULL && strcmp(command[i], " ")) {
        strcat(temp, " ");
        strcat(temp, command[i]);
        //printf("%s\n", temp);
        i++;
    }
    // Store the command in the history array
    strcpy(history[(*history_index) % MAX_HISTORY].line, temp);
    strcpy(temp, " ");
    history[(*history_index) % MAX_HISTORY].number = *history_index + 1; 
    // Update the history index and wrap around when reaching MAX_HISTORY
    (*history_index) = ((*history_index) + 1) % MAX_HISTORY;
}

/**
 * Prints the command history.
 * 
 * This function iterates through the command history array and prints the command number
 * and the corresponding command line. It skips any empty slots in the history array.
 * 
 * @param history The command history array.
 * @param history_index The index of the most recent command in the history array.
 */
void print_history(Command *history, int history_index) {
    int temp = 1;
    for (int i = 0; i < MAX_HISTORY; i++) {
        int index = (history_index + i) % MAX_HISTORY; 
        if (history[index].number != 0) { 
            printf("%d %s %d\n", temp, history[index].line, history[index].number);
            temp++;
        }
    }
}

//a wee bit of spaghetti code. need to refactor

char* get_command_from_history(char* input_buf, Command* history, int history_index) {
    char *ptr;
    int flag = 0;
    if (strcmp(input_buf, "!!\n") == 0) {
        strcpy(input_buf, history[(history_index-1) % MAX_HISTORY].line);
    } else if (input_buf[0] == '!') {
        int command_no;
        if (input_buf[1] == '-') {
            command_no = (history_index - strtol(input_buf + 2, &ptr, 10)) % MAX_HISTORY;
            if(command_no < 0)
                command_no += MAX_HISTORY;
        } 
        else {
            command_no = (history_index + strtol(input_buf + 1, &ptr, 10) -1) % MAX_HISTORY;
        }
        printf("attempted command %i%s %i history index %d\n", command_no, history[command_no].line, history[command_no].number, history_index);
        if (command_no >= 0 && command_no <= MAX_HISTORY) {
                strcpy(input_buf, history[command_no].line);
        } else {
            printf("Error: Invalid history invocation\n");
            // Set input_buf to an empty string to indicate an error
            flag = 1;
            input_buf[0] = '\0';
        }
        if(input_buf[0] == '\0' && flag != 1){
            printf("Error: Invalid history invocation\n");
        }
    }
    return input_buf;
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
            char error_msg[MAX_INPUT_LENGTH+strlen("cd: ")];
            strcpy(error_msg, "cd: ");
            strcat(error_msg, tokens[1]);
            perror(error_msg);
            return -1;
        }
    }
    return 0;
        
}