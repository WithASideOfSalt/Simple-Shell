#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Used for getcwd
#include "simple-shell.h"

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
    int history_index = 0;
    Command* history;
    //Load history here
    history = history_dup(load_history(&history_index), MAX_HISTORY);
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
             
            builtins command = get_enum(tokens[0]);
            //add any attempted command into the history
            add_to_history(tokens, history, &history_index);
            
            //check if built in command, if not let fork handle the rest
            switch(command){
                case CD:
                    printf("CD\n");
                    //Stage 3 stuff
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
                    printf("GETPATH\n");
                    break;
                case SETPATH:
                    printf("SETPATH\n");
                    break;
                case LAST_COMMAND:
                    printf("LAST_COMMAND\n");
                    break;
                case EXIT:
                    looping = 0;
                    break;
                default:
                    forky_fun(tokens[0], tokens+1, number_of_tokens-1);
                    printf("DEFAULT\n");
                    break;
            }
        }
        for (int i = 0; i < number_of_tokens; i++){
            free(tokens[i]);
        }
        free(tokens);
    }
    
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