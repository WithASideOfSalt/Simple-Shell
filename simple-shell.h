#define DELIMITERS " \t\n;&><|"
#define MAX_PATH_LENGTH 1028
#define MAX_INPUT_LENGTH 512
#define MAX_TOKENS 50
#define MAX_HISTORY 20

typedef enum builtins {CD, HISTORY, ALIAS, UNALIAS, GETPATH, SETPATH, LAST_COMMAND, EXIT, NONE} builtins;

typedef struct Command {
    int number;
    char line[MAX_INPUT_LENGTH];
} Command;

int tokenize(char *, char **);
builtins get_enum (char *);
int forky_fun(char *, char* [], int);
void add_to_history(char *, Command *, int *);
void print_history(Command *, int);
Command *load_history(int *);