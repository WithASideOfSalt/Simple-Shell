#define SIMPLE_SHELL_H
#define DELIMITERS " \t\n;&><|"
#define MAX_PATH_LENGTH 1028
#define MAX_INPUT_LENGTH 512
#define MAX_TOKENS 50
#define MAX_HISTORY 20
#define MAX_ALIASES 3


typedef enum builtins {CD, HISTORY, ALIAS, UNALIAS, GETPATH, SETPATH, EXIT, CLEARH, NONE} builtins;

typedef struct HistoryList {
    int index; // To store the current position in the circular buffer
    char command[MAX_HISTORY][MAX_INPUT_LENGTH];
} HistoryList;

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
HistoryList read_history();
HistoryList clear_history(HistoryList);
void save_history(HistoryList);
void add_to_history(char **, HistoryList *);
void print_history(HistoryList);
char* invoke_history(char*, HistoryList, int *, int *);

char* get_command_from_history(char *, Command *, int, int*);
int get_env(char **tokens, int number_of_tokens);
int set_env(char **tokens, int number_of_tokens);
int restore_original_path(char *original_path);
int changeDirectory(char **tokens, int number_of_tokens);
AliasList read_aliases();
int save_aliases(AliasList);
AliasList unalias(char* [], int, AliasList);
AliasList create_alias(char* [], int, AliasList);
void print_aliases();

char** ReplaceAliases(AliasList, int*, char**, int*);
char** subList(char **, int, int);
