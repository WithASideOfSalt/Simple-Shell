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
    int history_index = 0;
    Command* history = malloc(sizeof(Command) * MAX_HISTORY);    
    //Load history here
    history = load_history(&history_index);
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
      
        // Check if the input is a history invocation
        strcpy(input_buf,get_command_from_history(input_buf, history, history_index));
        
        // Create array of strings to store tokens
        char **tokens;
        // Allocate memory to the array of char pointers
        tokens = malloc(sizeof(char**)*MAX_TOKENS);
        // Parse input into tokens
        int number_of_tokens = tokenize(input_buf, tokens); 
        // Make sure that there are tokens / commands to process
        if (number_of_tokens > 0){ 
             
            builtins command = get_enum(tokens[0]);
            //add any attempted command into the history
            add_to_history(tokens, history, &history_index);
            
            //check if built in command, if not let fork handle the rest
            switch(command){
                case CD:
                    if(number_of_tokens == 1){
                        chdir(home_directory);
                    } else {
                        changeDirectory(tokens, number_of_tokens);
                    }
                    break;
                case HISTORY:
                    print_history(history, history_index);    
                    break;
                case ALIAS:
                    printf("ALIAS\n");
                    //Stage 7 stuff
                    break;
                case UNALIAS:
                    printf("UNALIAS\n");
                    //Stage 7 stuff
                    break;
                case GETPATH:
                    get_env(tokens, number_of_tokens);
                    break;
                case SETPATH:
                    set_env(tokens, number_of_tokens);
                    break;
                case EXIT:
                    looping = 0;
                    break;
                default:
                    forky_fun(tokens[0], tokens+1, number_of_tokens-1);
                    //printf("DEFAULT\n");
                    break;
            }
        }
        for (int i = 0; i < number_of_tokens; i++){
            free(tokens[i]);
        }
        free(tokens);
    }

    
    // Restore original path before exiting and directory
    restore_original_path(cwd);
    chdir(home_directory);
    save_history(history, &history_index);
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