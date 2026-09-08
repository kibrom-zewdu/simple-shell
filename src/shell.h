#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <limits.h>

#define SHELL_MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_ALIASES 100
#define MAX_PATH 1024
#define MAX_ENV_VARS 1000

#define MODE_INTERACTIVE 0
#define MODE_NON_INTERACTIVE 1
#define MODE_BATCH 2

typedef struct Command {
    char *args[MAX_ARGS];
    int arg_count;
    char *output_file;
    int background;
    int piped_to_next;
} Command;

typedef struct Alias {
    char *name;
    char *value;
} Alias;

typedef struct Shell {
    int mode;
    int exit_status;
    char *prompt;
    char **env_vars;
    int env_count;
    char *paths[MAX_PATH];
    int path_count;
    Alias aliases[MAX_ALIASES];
    int alias_count;
    char *oldpwd;
    char *pwd;
    pid_t shell_pid;
} Shell;

void init_shell(Shell *shell, int argc, char **argv, char **envp);
void run_shell(Shell *shell);
void cleanup_shell(Shell *shell);

int parse_input(Shell *shell, char *input, Command *commands, int max_commands);
void parse_command(Shell *shell, char *cmd_str, Command *cmd);
void parse_simple_command(Shell *shell, char *cmd_str, Command *cmd);
void expand_variables(Shell *shell, char *str);
char *trim_whitespace(char *str);

int is_builtin(char *cmd);
int execute_builtin(Shell *shell, Command *cmd);
int builtin_exit(Shell *shell, Command *cmd);
int builtin_cd(Shell *shell, Command *cmd);
int builtin_env(Shell *shell, Command *cmd);
int builtin_setenv(Shell *shell, Command *cmd);
int builtin_unsetenv(Shell *shell, Command *cmd);
int builtin_path(Shell *shell, Command *cmd);
int builtin_alias(Shell *shell, Command *cmd);
void print_error(void);

int execute_command(Shell *shell, Command *cmd);
int execute_external(Shell *shell, Command *cmd);
void wait_for_background(void);

char *get_env_var(Shell *shell, char *name);
void set_env_var(Shell *shell, char *name, char *value);
void unset_env_var(Shell *shell, char *name);
char *find_executable(Shell *shell, char *cmd);
void free_command(Command *cmd);

char *resolve_alias(Shell *shell, char *cmd);
void add_alias(Shell *shell, char *name, char *value);
void print_alias(Shell *shell, char *name);
void print_all_aliases(Shell *shell);

void setup_signal_handlers(void);

#endif