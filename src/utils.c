#include "shell.h"

char *get_env_var(Shell *shell, char *name) {
    char search[SHELL_MAX_INPUT];
    snprintf(search, sizeof(search), "%s=", name);
    size_t search_len = strlen(search);
    
    for (int i = 0; i < shell->env_count; i++) {
        if (shell->env_vars[i] && strncmp(shell->env_vars[i], search, search_len) == 0) {
            return shell->env_vars[i] + search_len;
        }
    }
    
    return NULL;
}

void set_env_var(Shell *shell, char *name, char *value) {
    char new_var[SHELL_MAX_INPUT];
    snprintf(new_var, sizeof(new_var), "%s=%s", name, value);
    
    char *existing = get_env_var(shell, name);
    if (existing) {
        for (int i = 0; i < shell->env_count; i++) {
            char search[SHELL_MAX_INPUT];
            snprintf(search, sizeof(search), "%s=", name);
            size_t search_len = strlen(search);
            
            if (shell->env_vars[i] && strncmp(shell->env_vars[i], search, search_len) == 0) {
                free(shell->env_vars[i]);
                shell->env_vars[i] = strdup(new_var);
                return;
            }
        }
    } else {
        if (shell->env_count < MAX_ENV_VARS - 1) {
            shell->env_vars[shell->env_count] = strdup(new_var);
            shell->env_count++;
            shell->env_vars[shell->env_count] = NULL;
        }
    }
}

void unset_env_var(Shell *shell, char *name) {
    char search[SHELL_MAX_INPUT];
    snprintf(search, sizeof(search), "%s=", name);
    size_t search_len = strlen(search);
    
    for (int i = 0; i < shell->env_count; i++) {
        if (shell->env_vars[i] && strncmp(shell->env_vars[i], search, search_len) == 0) {
            free(shell->env_vars[i]);
            
            for (int j = i; j < shell->env_count - 1; j++) {
                shell->env_vars[j] = shell->env_vars[j + 1];
            }
            
            shell->env_count--;
            shell->env_vars[shell->env_count] = NULL;
            return;
        }
    }
}

char *find_executable(Shell *shell, char *cmd) {
    if (strchr(cmd, '/')) {
        if (access(cmd, X_OK) == 0) {
            return strdup(cmd);
        }
        return NULL;
    }
    
    for (int i = 0; i < shell->path_count; i++) {
        char path[SHELL_MAX_INPUT];
        snprintf(path, sizeof(path), "%s/%s", shell->paths[i], cmd);
        
        if (access(path, X_OK) == 0) {
            return strdup(path);
        }
    }
    
    return NULL;
}

void free_command(Command *cmd) {
    for (int i = 0; i < cmd->arg_count; i++) {
        free(cmd->args[i]);
        cmd->args[i] = NULL;
    }
    cmd->arg_count = 0;
    
    free(cmd->output_file);
    cmd->output_file = NULL;
}

char *resolve_alias(Shell *shell, char *cmd) {
    char cmd_name[SHELL_MAX_INPUT];
    strncpy(cmd_name, cmd, sizeof(cmd_name) - 1);
    cmd_name[sizeof(cmd_name) - 1] = '\0';
    
    char *space = strchr(cmd_name, ' ');
    if (space) *space = '\0';
    
    for (int i = 0; i < shell->alias_count; i++) {
        if (strcmp(shell->aliases[i].name, cmd_name) == 0) {
            return shell->aliases[i].value;
        }
    }
    return NULL;
}

void add_alias(Shell *shell, char *name, char *value) {
    for (int i = 0; i < shell->alias_count; i++) {
        if (strcmp(shell->aliases[i].name, name) == 0) {
            free(shell->aliases[i].value);
            shell->aliases[i].value = strdup(value);
            return;
        }
    }
    
    if (shell->alias_count < MAX_ALIASES) {
        shell->aliases[shell->alias_count].name = strdup(name);
        shell->aliases[shell->alias_count].value = strdup(value);
        shell->alias_count++;
    }
}

void print_alias(Shell *shell, char *name) {
    for (int i = 0; i < shell->alias_count; i++) {
        if (strcmp(shell->aliases[i].name, name) == 0) {
            printf("%s='%s'\n", shell->aliases[i].name, shell->aliases[i].value);
            return;
        }
    }
}

void print_all_aliases(Shell *shell) {
    for (int i = 0; i < shell->alias_count; i++) {
        printf("%s='%s'\n", shell->aliases[i].name, shell->aliases[i].value);
    }
}