#include "simple-shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Used for getcwd




int main(void){
        
    char cwd[MAX_PATH_LENGTH];
    // Get the current working directory and check for NULL wd
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }
    // Save the original PATH when your shell starts up.

    if (getenv("PATH") != NULL) {
    strcpy(cwd, getenv("PATH"));
} else {
   printf("Error: PATH not found in the environment.\n");
  
}

char *home_directory = getenv("HOME");
if (home_directory != NULL) {
    if (chdir(home_directory) != 0) {
        perror("chdir() error");
       
    }
} else {
    printf("Error: HOME not found in the environment.\n");
    
}
// Print the current working directory after changing
if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("Current working dir: %s\n", cwd);
} else {
    perror("getcwd() error");

}
    

    char input_buf[MAX_INPUT_LENGTH];
    int looping = 1;
    //Main loop
    while (looping){
        // Print prompt
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
        // Create array of strings to store tokens
        char **tokens;
        // Allocate memory to the array of char pointers
        tokens = malloc(sizeof(char**)*MAX_TOKENS);
        // Parse input into tokens
        int number_of_tokens = tokenize(input_buf, tokens); 
        // Make sure that there are tokens / commands to process
        if (number_of_tokens > 0){ 
            if (strcmp(tokens[0], "exit") == 0){
                looping = 0;
            } 
            else if (strcmp(tokens[0], "getpath") == 0) {
            get_env(tokens, number_of_tokens);
        } 
        else if (strcmp(tokens[0], "setpath") == 0) {
            set_env(tokens, number_of_tokens);
        } 
        else if (strcmp(tokens[0], "cd") == 0 ) {
            if(number_of_tokens == 1){
                chdir(home_directory);
            } else{
                changeDirectory(tokens, number_of_tokens);
            }   
        }
        else {
                forky_fun(tokens[0], tokens+1, number_of_tokens-1);
            }
        }
        for (int i = 0; i < number_of_tokens; i++){
            free(tokens[i]);
        }
        free(tokens);
    }

    
    // Restore original path before exiting
    restore_original_path(cwd);
    

    return 0;
}


/*
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