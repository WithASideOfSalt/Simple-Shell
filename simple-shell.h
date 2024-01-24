#define SIMPLE_SHELL_H
#define DELIMITERS " \t\n;&><|"
#define MAX_PATH_LENGTH 1028
#define MAX_INPUT_LENGTH 512
#define MAX_TOKENS 50

int tokenize(char *, char **);
int forky_fun(char *, char* [], int);
int get_env(char **tokens, int number_of_tokens);
int set_env(char **tokens, int number_of_tokens);
int restore_original_path(char *original_path);