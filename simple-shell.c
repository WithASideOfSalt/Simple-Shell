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
    if (strcmp(command, "clearh") == 0)return CLEARH;
    return NONE;
}


/*
 *Loads the history from file.
 *@return The HistoryList loaded from .hist_list
 */
HistoryList read_history(){
    FILE *history_file;
    char buf[512];
    HistoryList history;

    history_file = fopen(".hist_list" , "r");
    if(history_file == NULL) {
      perror("Error opening history file");
      history.index = -1;
      return history;
   }
   int i = 0;
   while(fgets(buf, 512, history_file) != NULL) {
        char *token;
        token = strtok(NULL, DELIMITERS);
        int x = 0;
        while (token != NULL){
            strcat(history.command[i], token);
            token = strtok(NULL, DELIMITERS);
            x++;
        }
        i++;
   }
   printf("Imported %d saved history \n", i);
   fclose(history_file);
   return history;
}

HistoryList clear_history(HistoryList history){
    for(int i =0; i < MAX_HISTORY; i++){
        strcpy(history.command[i],"");
    }
    history.index = -1;
    return history;
}

void save_history(HistoryList history){
    FILE *history_file;
    history_file = fopen(".hist_list","w");
    if(history_file == NULL){
        perror("error opening file");
        return;
    }
    int x = (history.index + 1) % MAX_HISTORY;
    for(int i = 0; i<MAX_HISTORY; i++){
        if(strcmp(history.command[x],"") == 0){
            fprintf(history_file, "%s\n" ,history.command[history.index]);
        }
        x = (x + 1) % MAX_HISTORY; // wrap around when reaching MAX_HISTORY
    }
    fclose(history_file);
}

/**
 * Adds a command to the history array.
 *
 * @param command The command to be added to the history.
 * @param history The HistoryList representing the history.
 */
void add_to_history(char **command, HistoryList *history) {
    // Concatenate the command arguments into a single string
    char temp[MAX_INPUT_LENGTH] = "";
    int i = 0;
    while (command[i] != NULL && strcmp(command[i], " ")) {
        strcat(temp, " ");
        strcat(temp, command[i]);
        i++;
    }
    // Store the command in the history array
    strcpy(history->command[(history->index + 1) % MAX_HISTORY], temp);
    strcpy(temp, " ");
    // Update the history index and wrap around when reaching MAX_HISTORY
    history->index = (history->index + 1) % MAX_HISTORY;
}

/**
 * Prints the command history.
 * 
 * This function iterates through the command history array and prints the command number
 * and the corresponding command line. It skips any empty slots in the history array.
 * 
 * @param history The command history stucture.
 */
void print_history(HistoryList history) {
    printf("Print history index %d\n", history.index);
    int temp = 1;
    for (int i = 0; i < MAX_HISTORY; i++) {
        int index = (i + history.index + 1) % MAX_HISTORY; 
        if (strcmp(history.command[index], "") == 0) { 
            printf("%d %s %d\n", temp, history.command[index], index);
            temp++;
        }
    }
}

char* invoke_history(char* input_buf, HistoryList history, int *changed, int *fromHistory){
    char *ptr;
    char* new_buf= malloc(MAX_INPUT_LENGTH*sizeof(char));
    if (input_buf[1] == '!'){
        strcpy(new_buf, history.command[history.index]);
    } else if (input_buf[1] == '-'){
        int com = strtol(input_buf + 2, &ptr, 10);
        if (com > 0 && com <= 20){
            strcpy(new_buf, history.command[(history.index - com -1)%MAX_HISTORY]);
            *changed = 1;
            *fromHistory = 1;
        } else {
            printf("Failed in invoke history!\n");
            return input_buf;
        }
    } else {
        int com = strtol(input_buf + 4, &ptr, 10);
        if (com > 0 && com <= 20){
            int index = history.index + 1;
            while (strcmp(history.command[index], "") == 0){
                index ++;
            }
            strcpy(new_buf, history.command[(index + com + 1)%MAX_HISTORY]);
            *changed = 1;
            *fromHistory = 1;
        } else {
            printf("Failed in invoke history!\n");
            return input_buf;
        }
    }
    return new_buf;
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

/*
 *Loads the aliases from file.
 *@return The AliasList loaded from .aliases
 */
AliasList read_aliases(){
    FILE *alias_file;
    char buf[512];
    AliasList aliases;

    alias_file = fopen(".aliases" , "r");
    if(alias_file == NULL) {
      perror("Error opening alias file");
      aliases.length = 0;
      return aliases;
   }
   int i = 0;
   while(fgets(buf, 512, alias_file) != NULL) {
        char *token;
        token = strtok(buf, DELIMITERS);
        strcpy(aliases.list[i].to_replace, token);
        token = strtok(NULL, DELIMITERS);
        int x = 0;
        while (token != NULL){
            strcpy(aliases.list[i].replace_with[x], token);
            token = strtok(NULL, DELIMITERS);
            x++;
            
        }
        aliases.list[i].rplc_wth_size = x;
        i++;
   }
   aliases.length = i;
   printf("Imported %d saved aliases \n", aliases.length);
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

AliasList unalias(char* arguments[], int args_len, AliasList aliaslist){
    int found = 0;
    if(args_len != 1){
        printf("Error:incorrect usage of unalias command: Usage - unalias <alias>\n");
        return aliaslist;
    }
    for(int i=0;i<aliaslist.length;i++){
        //check if supplied alias is equal to any stored alias
        if(strcmp(aliaslist.list[i].to_replace, arguments[0]) == 0){
            printf("found a match\n");
            found = 1;
            //attempting to free memory used for the list by overwriting all the removed entry with the subsequent entry and repeat for the remainder of the list
            printf("list size = %d\n", aliaslist.length);
            for(int z=i;z<(aliaslist.length);z++){
                printf("counter = %d\n", z);
                printf("list size = %d\n", aliaslist.length);
                print_aliases(aliaslist);
                strcpy(aliaslist.list[z].to_replace, aliaslist.list[z+1].to_replace);
                for(int y=0;y<(aliaslist.list[i].rplc_wth_size); y++){
                    strcpy(aliaslist.list[z].replace_with[y], aliaslist.list[z+1].replace_with[y]);
                }
                aliaslist.list[z].rplc_wth_size = aliaslist.list[z+1].rplc_wth_size;
                printf("%d, is the rplcwithsize of alias %s \n", aliaslist.list[z].rplc_wth_size, aliaslist.list[z].to_replace);
                //setting the final entry to be empty so that it will be replaced by next added entry
                if(z == ((aliaslist.length-1))){
                    printf("length before = %d\n", aliaslist.length);
                    strcpy(aliaslist.list[z].to_replace, "");
                    printf("length before = %d\n", aliaslist.length);

                    for(int y=0;y<(aliaslist.list[z+1].rplc_wth_size-1); y++){
                        strcpy(aliaslist.list[z+1].replace_with[y], "");
                    }
                    aliaslist.list[z+1].rplc_wth_size = 0;
                    //reducing length to be equal to current length
                    aliaslist.length = (aliaslist.length-1);
                }
            }
            printf("%d\n", aliaslist.length);
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
            printf("create alias args_len = %d\n", args_len);
            aliaslist = unalias(arguments, 1, aliaslist);
        }
    }
    if(aliaslist.length < MAX_ALIASES){
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
    } else {
        printf("Error: alias list is full no more can be set \n");
    }
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
            printf("%s ", aliaslist.list[i].replace_with[z]);
        }
        printf("\n");
    }

}

char** ReplaceAliases(AliasList aliaslist, int* numtokens, char** tokens, int *changed){
    char **new_tokens;
    int newTokenNum = *numtokens;
    int counter = 1;
    int continue_loop = 1;
    int reset = 0;
    int foundFunction = 0;
    //ensure there is space for up to three aliases deep of tokens
    new_tokens = malloc(sizeof(char**) * (4 * (MAX_TOKENS)));
    new_tokens = tokens;
    //check for aliases in tokens[0] and alter them from aliases to the original command(s)
    //it lasts until it has looped 3 times or it doesnt find an alias in the list 
    while(counter < 5 && continue_loop == 1){
        for(int i =0; i < newTokenNum ; i++){
            for(int j =0; j< aliaslist.length; j++){
                //find a matching alias
                if(strcmp(aliaslist.list[j].to_replace, new_tokens[i]) == 0){
                    //get every token before and after the replacement
                    char** beforeSection = subList(new_tokens, 0, i);
                    char** afterSection = subList(new_tokens, i+1, newTokenNum);
                    char newSection[MAX_TOKENS][MAX_INPUT_LENGTH];
                    for(int c =0; c< MAX_TOKENS; c++){
                        for(int x = 0; x < MAX_INPUT_LENGTH; x++){
                            newSection[c][x] = aliaslist.list[j].replace_with[c][x];
                        }
                    }
                    //generate new token list
                    for(int c =0; c< i; c++){
                        new_tokens[c] = beforeSection[c];
                    }
                    for(int c = 0; c< aliaslist.list[j].rplc_wth_size; c++){
                        new_tokens[i+c] = newSection[c];
                        if(c >= 1){
                            newTokenNum += 1;
                        }
                    }
                    for(int c = 0; c< newTokenNum - i; c++){
                        new_tokens[i+aliaslist.list[j].rplc_wth_size+c] = afterSection[c];
                    }

                    free(beforeSection);
                    free(afterSection);

                    counter++;
                    i = 0;
                    j =0;
                    reset = 1;
                    foundFunction = 1;
                    break;
                }
            }
            if(reset == 1){
                break;
            }
        }
        //printf("Gotten to the end with repalcement, reset = %d\n", reset);
        if(reset == 0){
            continue_loop = 0;
        }
        reset = 0;
    }
    if(counter > 4){
        printf("The entered alias has attempted to loop 4 or more times which is past the limit please alter aliases \n");
        return tokens;
    }
    if(foundFunction == 1){
        *numtokens = newTokenNum;
        *changed = 1;
        return new_tokens;
    }
    else{
        return tokens;
    }
}

char** subList(char** list, int start, int end){
    char** subList = malloc(sizeof(char**) * (4 * (MAX_TOKENS)));
    int pos =0;
    for(int i =start; i < end; i++){
        subList[pos] = list[i];
        pos++;
    }
    return subList;
}
