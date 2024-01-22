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

AliasList read_aliases(){
    FILE *alias_file;
    char buf[512];
    AliasList aliases;

    alias_file = fopen(".aliases" , "r");
    if(alias_file == NULL) {
      perror("Error opening alias file");
   }
   int i = 0;
   while(fgets(buf, 512, alias_file) != NULL) {
        char *token;
        token = strtok(buf, DELIMITERS);
        strcpy(aliases.list[i].to_replace, token);
        token = strtok(NULL, DELIMITERS);
        int x = 0;
        while (token != NULL){
            printf("x:%d\n", x);
            strcpy(aliases.list[i].replace_with[x], token);
            token = strtok(NULL, DELIMITERS);
            x++;
            
        }
        aliases.list[i].rplc_wth_size = x;
        printf("i:%d\n", i);
        i++;
   }
   printf("i:%d\n", i);
   aliases.length = i;
   printf("aliases length:%d\n", aliases.length);
   fclose(alias_file);
   return aliases;
}

int save_aliases(AliasList a_list){
    FILE *alias_file;
    alias_file = fopen(".aliases" , "w");
    for (int alias = 0; alias < a_list.length; alias++){
        char line[MAX_INPUT_LENGTH];
        memset(line,0,strlen(line)); // Make sure line is empty
        strcat(line, a_list.list[alias].to_replace);
        for (int i = 0; i<a_list.list[alias].rplc_wth_size; i++){
            strcat(line, " ");
            strcat(line, a_list.list[alias].replace_with[i]);
        }
        strcat(line, "\n");
        fprintf(alias_file, "%s", line);
    }
    return 0;
}
