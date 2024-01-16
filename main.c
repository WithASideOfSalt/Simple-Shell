#include <stdio.h>
#include <string.h>
#include <unistd.h> // Used for getcwd

int main(void){
    
    char cwd[1028];
    getcwd(cwd, sizeof(cwd)); // Change this 
    char input_buf[512];
    char* delimiters = " \t\n;&><|";
    char* token;
    int looping = 1;
    // Do while shell has not terminated
    while(looping){
        // Display prompt
        printf("8--");
        // Read and parse user input
        fgets(input_buf, sizeof(input_buf), stdin);
        token = strtok(input_buf, delimiters);
        while( token != NULL ) {
        printf( "\"%s\"\n", token );
        if (strcmp(token, "exit") == 0){
            printf("Exit\n");
            looping = 0;
        }
        token = strtok(NULL, delimiters);
        }
    }
    // Exit
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