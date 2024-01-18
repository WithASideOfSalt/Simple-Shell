#include <stdio.h>
#include <string.h>
#include <unistd.h> // Used for getcwd

#define MAX_PATH_LENGTH 1028
#define MAX_INPUT_LENGTH 512
#define DELIMITERS " \t\n;&><|"

int main(void){
    
    char cwd[MAX_PATH_LENGTH];
    // Get the current working directory and check for NULL wd
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }
    printf("Current working dir: %s\n", cwd); //Test current working directory
    char input_buf[MAX_INPUT_LENGTH];
    char *token;
    int looping = 1;

    //Main loop
    while (looping){
      printf("--8 ");
      // Get input from stdin and check for error
      if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
            //check for CTRL^D
            if(feof(stdin)){
                strcpy(input_buf, "exit");
                printf("%s\n", input_buf);
            }
            clearerr(stdin);            
        }
    
      // Parse input into tokens
      token = strtok(input_buf, DELIMITERS);
        while( token != NULL ) {
            printf( "\"%s\"\n", token );
            if (strcmp(token, "exit") == 0){
                printf("Exit\n");
                looping = 0;
            }
            token = strtok(NULL, DELIMITERS);
        }
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