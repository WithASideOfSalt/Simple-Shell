#define DELIMITERS " \t\n;&><|"
#define MAX_PATH_LENGTH 1028
#define MAX_INPUT_LENGTH 512
#define MAX_TOKENS 50
#define MAX_ALIASES 10

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
int forky_fun(char *, char* [], int);
void unalias(char* [], int, AliasList);
void create_alias(char* [], int, AliasList);
void print_aliases(AliasList);