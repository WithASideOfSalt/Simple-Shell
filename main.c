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
    AliasList aliaslist;
    aliaslist.length = 0
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
            //check for aliases in tokens[0] and alter them from aliases to the original command(s)
            for(i =0; i<aliaslist.length; i++){
                if (strcmp(aliaslist.list[i].to_replace, tokens[0]) == 0){
                    char **new_tokens;
                    //ensure there is space for new tokens
                    new_tokens = malloc(sizeof(char*)*(2*(MAX_TOKENS)));
                    for(z = 0; z<aliaslist.list[i].rplc_wth_size; z++){
                        //append new tokens to start of the new token list
                        new_tokens[z] = aliaslist.list[i].replace_with[z]
                    }
                    //repeating for all tokens except one as that one has been replaced by its alias
                    //replace with size is reduced by 1 so that it is the index rather than the number
                    for(z = (aliaslist.list[i].rplc_wth_size-1); z<(number_of_tokens-1)+(aliaslist.list[i].rplc_wth_size-1); z++){
                        new_tokens[z] = tokens[z-aliaslist.list[i].rplc_wth_size];
                    }
                } 
            }
            //after subbing in checking for exit at position 0
            if (strcmp(tokens[0], "exit") == 0){
                looping = 0;
            }
            if(strcmp(tokens[0], "unalias") == 0){
                unalias(tokens+1, number_of_tokens-1, aliaslist);
            } 
            if (strcmp(tokens[0], "alias") == 0){
                if(number_of_tokens > 1){
                    create_alias(tokens+1, number_of_tokens-1, aliaslist); 
                } else {
                    print_aliases(aliaslist);
                }
                             
            } else {
                forky_fun(tokens[0], tokens+1, number_of_tokens-1);
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