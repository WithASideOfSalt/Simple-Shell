#define SIMPLE_SHELL_H
#define DELIMITERS " \t\n;&><|"
#define MAX_PATH_LENGTH 1028
#define MAX_INPUT_LENGTH 512
#define MAX_TOKENS 50
#define MAX_HISTORY 20
#define MAX_ALIASES 3


typedef enum builtins {CD, HISTORY, ALIAS, UNALIAS, GETPATH, SETPATH, EXIT, CLEARH, NONE} builtins;

typedef struct Command {
    int number;
    char line[MAX_INPUT_LENGTH];
} Command;

typedef struct History{
    Command commands[MAX_HISTORY];
    int history_index;
    int maxReached;
}History;

typedef struct Alias{
    char to_replace[MAX_INPUT_LENGTH];
    char replace_with[MAX_TOKENS][MAX_INPUT_LENGTH];
    int rplc_wth_size;
} Alias;

typedef struct AliasList{
    Alias list[MAX_ALIASES];
    int length;
} AliasList;

int tokenize(char *, char **);
builtins get_enum (char *);
int forky_fun(char *, char* [], int);
History add_to_history(char **, History);
void print_history(History);
History load_history();
void save_history(History);
char* get_command_from_history(char *, History , int*);
int get_env(char **tokens, int number_of_tokens);
int set_env(char **tokens, int number_of_tokens);
int restore_original_path(char *original_path);
int changeDirectory(char **tokens, int number_of_tokens);
AliasList read_aliases();
int save_aliases(AliasList);
AliasList unalias(char* [], int, AliasList);
AliasList create_alias(char* [], int, AliasList);
void print_aliases();
History clear_history(History);
char** ReplaceAliases(AliasList, int*, char**);
char** subList(char **, int, int);
