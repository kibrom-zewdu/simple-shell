#include "shell.h"

void init_shell(Shell *shell, int argc, char **argv, char **envp) {
    (void)argv;
    
    memset(shell, 0, sizeof(Shell));
    
    if (argc == 1) {
        if (isatty(STDIN_FILENO)) {
            shell->mode = MODE_INTERACTIVE;
            shell->prompt = "$ ";
        } else {
            shell->mode = MODE_NON_INTERACTIVE;
            shell->prompt = NULL;
        }
    } else if (argc == 2) {
        shell->mode = MODE_BATCH;
        shell->prompt = NULL;
    } else {
        print_error();
        exit(1);
    }
    
    shell->shell_pid = getpid();
    shell->env_vars = malloc(MAX_ENV_VARS * sizeof(char *));
    shell->env_count = 0;
    
    for (int i = 0; envp[i] != NULL && i < MAX_ENV_VARS - 1; i++) {
        shell->env_vars[shell->env_count] = strdup(envp[i]);
        shell->env_count++;
    }
    shell->env_vars[shell->env_count] = NULL;
    
    shell->paths[0] = strdup("/bin");
    shell->paths[1] = strdup("/usr/bin");
    shell->paths[2] = strdup("/usr/local/bin");
    shell->path_count = 3;
    
    shell->pwd = getcwd(NULL, 0);
    if (shell->pwd == NULL) {
        shell->pwd = strdup("/");
    }
    shell->oldpwd = strdup(shell->pwd);
    
    set_env_var(shell, "PWD", shell->pwd);
    set_env_var(shell, "OLDPWD", shell->oldpwd);
    
    shell->exit_status = 0;
    
    if (shell->mode == MODE_INTERACTIVE) {
        setup_signal_handlers();
    }
}

void run_shell(Shell *shell) {
    char input[SHELL_MAX_INPUT];
    FILE *input_stream = stdin;
    
    if (shell->mode == MODE_BATCH) {
        char *arg1 = get_env_var(shell, "1");
        if (!arg1) {
            print_error();
            exit(1);
        }
        
        input_stream = fopen(arg1, "r");
        if (!input_stream) {
            print_error();
            exit(1);
        }
    }
    
    while (1) {
        if (shell->mode == MODE_INTERACTIVE && shell->prompt) {
            write(STDOUT_FILENO, shell->prompt, strlen(shell->prompt));
            fflush(stdout);
        }
        
        if (!fgets(input, sizeof(input), input_stream)) {
            if (shell->mode == MODE_INTERACTIVE) {
                if (feof(input_stream)) {
                    write(STDOUT_FILENO, "\n", 1);
                }
                write(STDOUT_FILENO, "\n", 1);
            }
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        Command commands[MAX_ARGS];
        memset(commands, 0, sizeof(commands));
        int cmd_count = parse_input(shell, input, commands, MAX_ARGS);
        
        for (int i = 0; i < cmd_count; i++) {
            int should_execute = 1;
            
            if (i > 0) {
                if (commands[i-1].piped_to_next == 1) { // &&
                    should_execute = (shell->exit_status == 0);
                } else if (commands[i-1].piped_to_next == 2) { // ||
                    should_execute = (shell->exit_status != 0);
                }
            }
            
            if (should_execute) {
                shell->exit_status = execute_command(shell, &commands[i]);
            } else {
                if (commands[i-1].piped_to_next == 2) { // ||
                    shell->exit_status = 0;
                }
            }
            
            free_command(&commands[i]);
        }
        
        if (shell->mode == MODE_BATCH && shell->exit_status == -1) {
            break;
        }
        
        wait_for_background();
    }
    
    if (shell->mode == MODE_BATCH && input_stream != stdin) {
        fclose(input_stream);
    }
}

void cleanup_shell(Shell *shell) {
    for (int i = 0; i < shell->env_count; i++) {
        free(shell->env_vars[i]);
    }
    free(shell->env_vars);
    
    for (int i = 0; i < shell->path_count; i++) {
        free(shell->paths[i]);
    }
    
    for (int i = 0; i < shell->alias_count; i++) {
        free(shell->aliases[i].name);
        free(shell->aliases[i].value);
    }
    
    free(shell->pwd);
    free(shell->oldpwd);
}

void print_error(void) {
    write(STDERR_FILENO, "An error has occurred\n", 22);
}