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
    AliasList aliaslist = read_aliases();
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
            int found_function = 0;
            int found_alias = 0;
            int continue_loop = 1;
            int counter = 1;
            char **new_tokens;
            //check for aliases in tokens[0] and alter them from aliases to the original command(s)
            while (continue_loop == 1 && counter < 4)
            {
                if(counter == 1){
                    //if counter is at 1 that means the input will be tokens from the original input
                    //so will malloc correct space for new_tokens and swap tokens to new_tokens
                   new_tokens = malloc(sizeof(char*) * (2 * (MAX_TOKENS)));
                    for(int z=0;z<number_of_tokens;z++){
                        new_tokens[z] = tokens[z];
                    }
                    printf("swapped tokens to new tokens and this is the number of tokens: %d \n", number_of_tokens);
                } else {
                    //since not counter 1 that means that it has to be the second or higher loop so has to make new_tokens have more space incase of max tokens in alias
                    found_alias = 0;
                    //creates a temp array to store the new_tokens data temporarily while deleting then re-creating new_tokens array
                    char** temp_tokens;
                    temp_tokens = malloc(sizeof(char*) * ((counter+1) * (MAX_TOKENS)));
                    for(int z=0;z<number_of_tokens;z++){
                        temp_tokens[z] = new_tokens[z];
                    }
                    free(new_tokens);
                    new_tokens = malloc(sizeof(char*) * ((counter+1) * (MAX_TOKENS)));
                    for(int z=0;z<number_of_tokens;z++){
                        new_tokens[z] = temp_tokens[z];
                    }
                    free(temp_tokens);  
                    printf("expanded memory storage for new_tokens \n");
                }
                for(int i=0; i<aliaslist.length; i++){   
                    printf("gets to index %d \n", i);  
                    if (strcmp(aliaslist.list[i].to_replace, new_tokens[0]) == 0 && found_alias == 0){
                        printf("gets a comparison \n");
                        //creating new integer for current number of tokens
                        int new_number_of_tokens = (number_of_tokens+aliaslist.list[i].rplc_wth_size);
                        //ensure there is space for new tokens
                        for(int z=0; z<aliaslist.list[i].rplc_wth_size; z++){
                            //append new tokens to start of the new token list
                            new_tokens[z] = aliaslist.list[i].replace_with[z];
                            //if finished adding new tokens add old tokens on the end      
                            if(z == (aliaslist.list[i].rplc_wth_size-1)){
                                //repeat equal to the number of tokens -z so that exactly the max index is used
                                for(int y=1; y<((new_number_of_tokens)-z); y++){
                                    new_tokens[z+y] = tokens[y];
                                }
                            }
                        }
                        //if on the first loop free tokens because it wont be needed any more
                        if(counter == 1){
                            free(tokens);
                        }
                        found_alias = 1;
                        found_function = 1;
                        number_of_tokens = new_number_of_tokens-1;
                        printf("found alias on loop %d \n", i);                
                    } 
                }
                if(found_alias == 0){
                    continue_loop = 0;
                }
                printf("gets to the end of the while \n");
                counter++;
            }
            //if ever found an alias once or up to three times fork
            if(found_function == 1){
                for(int i=0;i<number_of_tokens;i++){
                    printf("token %d is %s and this is the number of tokens %d \n", i, new_tokens[i], number_of_tokens);
                }
                forky_fun(new_tokens[0], new_tokens+1, (number_of_tokens-1));
            }
            //after subbing in checking for exit at position 0
            if (strcmp(tokens[0], "exit") == 0){
                looping = 0;
                found_function = 1;
            }
            if(strcmp(tokens[0], "unalias") == 0){
                aliaslist = unalias(tokens+1, number_of_tokens-1, aliaslist);
                found_function = 1;
            } 
            if (strcmp(tokens[0], "alias") == 0){
                if(number_of_tokens > 1){
                    if(number_of_tokens == 2){
                        printf("Error: please enter a command to run/the alias you want it to be. \n");
                    } else{
                        aliaslist = create_alias(tokens+1, number_of_tokens-1, aliaslist); 
                    }   
                } else {
                    print_aliases(aliaslist);
                }
                found_function = 1;
                             
            } 
            if (found_function == 0){
                forky_fun(tokens[0], tokens+1, number_of_tokens-1);
            }
        }
        for (int i = 0; i < number_of_tokens; i++){
            free(tokens[i]);
        }
        free(tokens);
    }
    save_aliases(aliaslist);
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