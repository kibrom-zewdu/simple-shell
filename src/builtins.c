#include "shell.h"
#include <limits.h>

int is_builtin(char *cmd) {
    if (!cmd) return 0;
    
    return (strcmp(cmd, "exit") == 0 ||
            strcmp(cmd, "cd") == 0 ||
            strcmp(cmd, "env") == 0 ||
            strcmp(cmd, "setenv") == 0 ||
            strcmp(cmd, "unsetenv") == 0 ||
            strcmp(cmd, "path") == 0 ||
            strcmp(cmd, "alias") == 0);
}

int execute_builtin(Shell *shell, Command *cmd) {
    if (cmd->arg_count == 0) return 0;
    
    char *builtin = cmd->args[0];
    
    if (strcmp(builtin, "exit") == 0) {
        return builtin_exit(shell, cmd);
    } else if (strcmp(builtin, "cd") == 0) {
        return builtin_cd(shell, cmd);
    } else if (strcmp(builtin, "env") == 0) {
        return builtin_env(shell, cmd);
    } else if (strcmp(builtin, "setenv") == 0) {
        return builtin_setenv(shell, cmd);
    } else if (strcmp(builtin, "unsetenv") == 0) {
        return builtin_unsetenv(shell, cmd);
    } else if (strcmp(builtin, "path") == 0) {
        return builtin_path(shell, cmd);
    } else if (strcmp(builtin, "alias") == 0) {
        return builtin_alias(shell, cmd);
    }
    
    return 1;
}

int builtin_exit(Shell *shell, Command *cmd) {
    int exit_code = 0;
    
    if (cmd->arg_count > 2) {
        print_error();
        return 1;
    }
    
    if (cmd->arg_count == 2) {
        char *endptr;
        long exit_code_long = strtol(cmd->args[1], &endptr, 10);
        if (*endptr != '\0' || exit_code_long < INT_MIN || exit_code_long > INT_MAX) {
            print_error();
            return 1;
        }
        exit_code = (int)exit_code_long;
    }
    
    shell->exit_status = -1;
    int code_to_exit = exit_code;
    cleanup_shell(shell);
    exit(code_to_exit);
}

int builtin_cd(Shell *shell, Command *cmd) {
    char *target = NULL;
    
    if (cmd->arg_count > 2) {
        print_error();
        return 1;
    }
    
    if (cmd->arg_count == 1) {
        target = get_env_var(shell, "HOME");
        if (!target) {
            print_error();
            return 1;
        }
    } else {
        target = cmd->args[1];
        
        if (strcmp(target, "-") == 0 || strcmp(target, "--") == 0) {
            target = get_env_var(shell, "OLDPWD");
            if (!target) {
                print_error();
                return 1;
            }
            
            if (strcmp(cmd->args[1], "-") == 0) {
                printf("%s\n", target);
            }
        }
    }
    
    free(shell->oldpwd);
    shell->oldpwd = strdup(shell->pwd ? shell->pwd : "");
    if (shell->oldpwd) {
        set_env_var(shell, "OLDPWD", shell->oldpwd);
    }
    
    if (chdir(target) != 0) {
        print_error();
        return 1;
    }
    
    free(shell->pwd);
    shell->pwd = getcwd(NULL, 0);
    if (shell->pwd) {
        set_env_var(shell, "PWD", shell->pwd);
    }
    
    return 0;
}

int builtin_env(Shell *shell, Command *cmd) {
    (void)cmd;
    
    for (int i = 0; i < shell->env_count; i++) {
        if (shell->env_vars[i]) {
            printf("%s\n", shell->env_vars[i]);
        }
    }
    
    return 0;
}

int builtin_setenv(Shell *shell, Command *cmd) {
    if (cmd->arg_count != 3) {
        print_error();
        return 1;
    }
    
    set_env_var(shell, cmd->args[1], cmd->args[2]);
    return 0;
}

int builtin_unsetenv(Shell *shell, Command *cmd) {
    if (cmd->arg_count != 2) {
        print_error();
        return 1;
    }
    
    unset_env_var(shell, cmd->args[1]);
    return 0;
}

int builtin_path(Shell *shell, Command *cmd) {
    for (int i = 0; i < shell->path_count; i++) {
        free(shell->paths[i]);
        shell->paths[i] = NULL;
    }
    shell->path_count = 0;
    
    if (cmd->arg_count == 1) {
        return 0;
    }
    
    for (int i = 1; i < cmd->arg_count; i++) {
        if (shell->path_count < MAX_PATH) {
            shell->paths[shell->path_count] = strdup(cmd->args[i]);
            if (shell->paths[shell->path_count]) {
                shell->path_count++;
            }
        }
    }
    
    return 0;
}

int builtin_alias(Shell *shell, Command *cmd) {
    if (cmd->arg_count == 1) {
        print_all_aliases(shell);
        return 0;
    }
    
    for (int i = 1; i < cmd->arg_count; i++) {
        char *arg = cmd->args[i];
        char *equals = strchr(arg, '=');
        
        if (equals) {
            size_t name_len = equals - arg;
            char *name = malloc(name_len + 1);
            char *value = strdup(equals + 1);
            
            if (name && value) {
                strncpy(name, arg, name_len);
                name[name_len] = '\0';
                
                if (value[0] == '\'') {
                    memmove(value, value + 1, strlen(value));
                    if (value[strlen(value)-1] == '\'') {
                        value[strlen(value)-1] = '\0';
                    }
                }
                
                add_alias(shell, name, value);
            }
            if (name) free(name);
            if (value) free(value);
        } else {
            print_alias(shell, arg);
        }
    }
    
    return 0;
}