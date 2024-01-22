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
AliasList unalias(char* arguments[], int args_len, AliasList aliaslist){
    int found = 0;
    for(int i=0;i<aliaslist.length;i++){
        //check if supplied alias is equal to any stored alias
        if(strcmp(aliaslist.list[i].to_replace, arguments[0]) == 0){
            found = 1;
            //attempting to free memory used for the list by overwriting all the removed entry with the subsequent entry and repeat for the remainder of the list
            for(int z=i;z<(aliaslist.length-1);z++){
                strcpy(aliaslist.list[z].to_replace, aliaslist.list[z+1].to_replace);
                for(int y=0;y<(aliaslist.list[i].rplc_wth_size-1); y++){
                    strcpy(aliaslist.list[z].replace_with[y], aliaslist.list[z+1].replace_with[y]);
                }
                aliaslist.list[z].rplc_wth_size = aliaslist.list[z+1].rplc_wth_size;
            }
            //setting the final entry to be empty so that it will be replaced by next added entry
            strcpy(aliaslist.list[aliaslist.length].to_replace, "");
            //reducing length to be equal to current length
            aliaslist.length = (aliaslist.length-1);
            return aliaslist;
        }
    }
    if(found == 0){
        printf("Error: alias not found \n");
    }
    return aliaslist;
}
//the code to create new aliases this does not save them to any other file it just adds them for the current run
AliasList create_alias(char* arguments[], int args_len, AliasList aliaslist){
    //check for attempted creation of duplicate aliases
    for(int i=0;i<aliaslist.length;i++){
        if(strcmp(aliaslist.list[i].to_replace, arguments[0]) == 0){
            printf("Error: creation of duplicate alias:%s replaced older alias \n", arguments[0]);
            //remove old alias
            aliaslist = unalias(arguments, args_len, aliaslist);
        }
    }
    //create new instance of struct
    Alias newalias;
    //populate struct
    newalias.rplc_wth_size = (args_len-1);
    strcpy(newalias.to_replace, arguments[0]);
    for(int i=0; i<(args_len-1); i++){
        strcpy(newalias.replace_with[i], arguments[i+1]);
    }
    //update aliaslist
    aliaslist.list[aliaslist.length] = newalias;
    aliaslist.length = (aliaslist.length+1); 
    return aliaslist;
}

void print_aliases(AliasList aliaslist){
    //check if there are no aliases
    if(aliaslist.length == 0){
        printf("Error: no aliases found \n");
    }
    //print all aliases
    for(int i=0;i<aliaslist.length;i++){
        printf("Alias %d: %s is replaced by ", (i+1), aliaslist.list[i].to_replace);
        for(int z=0;z<aliaslist.list[i].rplc_wth_size; z++){
            printf("%s", aliaslist.list[i].replace_with[z]);
        }
        printf("\n");
    }

}