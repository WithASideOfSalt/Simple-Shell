#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Used for getcwd
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_PATH_LENGTH 1028
#define MAX_INPUT_LENGTH 512
#define DELIMITERS " \t\n;&><|"

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


int main(void){
    
    char cwd[MAX_PATH_LENGTH];
    // Get the current working directory and check for NULL wd
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }
    printf("Current working dir: %s\n", cwd); //Test current working directory
    char input_buf[MAX_INPUT_LENGTH];
    int looping = 1;
    //Main loop
    while (looping){
        printf("8-- ");
        // Get input from stdin and check for error
        if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
            //check for ctrl-D
            if(feof(stdin)){
                strcpy(input_buf, "exit");
                printf("\n");
            }
            clearerr(stdin);            
        }
        // Parse input into tokens
        char **tokens;
        tokens = malloc(sizeof(char**)*50);
        int number_of_tokens = tokenize(input_buf, tokens);

        if (number_of_tokens > 0){ 
            if (strcmp(tokens[0], "exit") == 0){
                looping = 0;
            } else {
                forky_fun(tokens[0], tokens+1, number_of_tokens-1);
            }
        }
    }
    
    return 0;
}


/*
Max of 26 input characters, 27 causes malloc(): corrupted top size

Find the user home directory from the environment
Set current working directory to user home directory
Save the current path
Load history
Load aliases



While the command is a history invocation or alias then replace it with the
appropriate command from history or the aliased command respectively
If command is built-in invoke appropriate function
Else execute command as an external process
End while
Save history
Save aliases
Restore original path




*/