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

        int fromHistory = 0;
        // Check if the input is a history invocation
        strcpy(input_buf,get_command_from_history(input_buf, history, history_index, &fromHistory));
        
        // Create array of strings to store tokens
        char **tokens;
        // Allocate memory to the array of char pointers
        tokens = malloc(sizeof(char**)* MAX_TOKENS);
        // Parse input into tokens
        int number_of_tokens = tokenize(input_buf, tokens); 
        // Make sure that there are tokens / commands to process
        if (number_of_tokens > 0){ 
            if(fromHistory == 0){
                add_to_history(tokens, history, &history_index);
            }   
            char **new_tokens;
            int counter = 1;
            int continue_loop = 1;
            int found_alias_this_loop;
            int new_number_of_tokens;
            int found_function = 0;
            //ensure there is space for up to three aliases deep of tokens
            new_tokens = malloc(sizeof(char**) * (4 * (MAX_TOKENS)));
            //check for aliases in tokens[0] and alter them from aliases to the original command(s)
            //it lasts until it has looped 3 times or it doesnt find an alias in the list 
            while(counter < 5 && continue_loop == 1){
                //if looped 3 times increase counter so can throw error message
                if(counter == 4){
                    counter++;
                }
                //temporary variable just to track whether a function has been found this loop
                found_alias_this_loop = 0;
                //repeats for length of array list to find alias
                for(int i=0; i<aliaslist.length; i++){
                    if (strcmp(aliaslist.list[i].to_replace, tokens[0]) == 0){
                        //creating new integer for current number of tokens
                        new_number_of_tokens = ((number_of_tokens-1)+aliaslist.list[i].rplc_wth_size);
                        for(int z=0; z<aliaslist.list[i].rplc_wth_size; z++){
                            //append new tokens to start of the new token list
                            new_tokens[z] = aliaslist.list[i].replace_with[z];
                            //adding null terminator to end of token
                            strcat(new_tokens[z], "\0");
                            //if finished adding new tokens add old tokens on the end      
                            if(z == (aliaslist.list[i].rplc_wth_size-1)){
                                //repeat equal to the number of tokens -z so that exactly the max index is used
                                for(int y=1; y<((new_number_of_tokens)-z); y++){
                                    strcpy(new_tokens[z+y], tokens[y]);
                                }
                            }
                            //replacing tokens with new_tokens so that it may loop properly
                        }
                        for(int x = 0; x < (new_number_of_tokens); x++){
                            strcpy(tokens[x], new_tokens[x]);
                        }
                        printf("what is in tokens\n");
                        for(int i =0; i < 2; i++){
                            printf("%s\n",tokens[i]);
                        }
                        //repeating for all tokens except one as that one has been replaced by its alias
                        //replace with size is reduced by 1 so that it is the index rather than the number
                        found_function = 1;
                        //setting found alias to one
                        found_alias_this_loop = 1;
                    } 
                }
                printf("%s\n", *tokens);
                //end loop if havent found an alias in the loop
                if(found_alias_this_loop == 0){
                    continue_loop = 0;
                }
                //increment counter
                counter++;
            }
            if(counter > 4){
                printf("The entered alias has attempted to loop 4 or more times which is past the limit please alter aliases \n");
            }
            if(found_function == 1 && counter <= 4){
                forky_fun(new_tokens[0], new_tokens+1, new_number_of_tokens);
            } 
            builtins command = get_enum(tokens[0]);
            
            //check if command has already been found with aliases
            if(found_function == 0){
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
                        if(number_of_tokens > 1){
                            printf("Error: incorrect usage of history. Usage: history");
                        }else{
                            print_history(history, history_index);  
                        }  
                        break;
                    case ALIAS:
                        if(number_of_tokens > 1){
                            if(number_of_tokens == 2){
                                printf("Error: please enter a command to run/the alias you want it to be. \n");
                            } else{
                                aliaslist = create_alias(tokens+1, number_of_tokens-1, aliaslist); 
                            }   
                        } else {
                            print_aliases(aliaslist);
                        }
                        break;
                    case UNALIAS:
                        aliaslist = unalias(tokens+1, number_of_tokens-1, aliaslist);
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
                    case CLEARH:
                        clear_history(history, &history_index);
                        break;
                    default:
                        forky_fun(tokens[0], tokens+1, number_of_tokens-1);
                        //printf("DEFAULT\n");
                        break;
                }
            }
        }
        //printf("Before Free\n");
        for (int i = 0; i < number_of_tokens; i++){
            memset(tokens[i],0,strlen(tokens[i]));
            free(tokens[i]);
        }
        free(tokens);
        //printf("After free");
    }

    
    // Restore original path before exiting and directory
    restore_original_path(cwd);
    chdir(home_directory);
    save_history(history, &history_index);
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