#include "shell.h"

int parse_input(Shell *shell, char *input, Command *commands, int max_commands) {
    char *input_copy = strdup(input);
    char *saveptr;
    int cmd_count = 0;
    
    if (!input_copy) return 0;
    
    char *comment = strchr(input_copy, '#');
    if (comment) *comment = '\0';
    
    char *token = strtok_r(input_copy, ";", &saveptr);
    
    while (token && cmd_count < max_commands) {
        char *cmd_str = trim_whitespace(token);
        
        if (strlen(cmd_str) > 0) {
            char *segment = strdup(cmd_str);
            char *subtoken = segment;
            
            while (subtoken && cmd_count < max_commands) {
                char *next_and = strstr(subtoken, "&&");
                char *next_or = strstr(subtoken, "||");
                char *next_operator = NULL;
                int operator_type = 0;
                
                if (next_and && (!next_or || next_and < next_or)) {
                    next_operator = next_and;
                    operator_type = 1;
                } else if (next_or) {
                    next_operator = next_or;
                    operator_type = 2;
                }
                
                if (next_operator) {
                    *next_operator = '\0';
                    char *command_part = trim_whitespace(subtoken);
                    
                    if (strlen(command_part) > 0) {
                        memset(&commands[cmd_count], 0, sizeof(Command));
                        parse_command(shell, command_part, &commands[cmd_count]);
                        commands[cmd_count].piped_to_next = operator_type;
                        cmd_count++;
                    }
                    
                    subtoken = next_operator + 2;
                } else {
                    char *command_part = trim_whitespace(subtoken);
                    if (strlen(command_part) > 0) {
                        memset(&commands[cmd_count], 0, sizeof(Command));
                        parse_command(shell, command_part, &commands[cmd_count]);
                        cmd_count++;
                    }
                    break;
                }
            }
            
            free(segment);
        }
        
        token = strtok_r(NULL, ";", &saveptr);
    }
    
    free(input_copy);
    return cmd_count;
}

void parse_command(Shell *shell, char *cmd_str, Command *cmd) {
    char *work_str = strdup(cmd_str);
    if (!work_str) return;
    
    char *bg_pos = strrchr(work_str, '&');
    if (bg_pos && bg_pos > work_str && *(bg_pos-1) != '&' && *(bg_pos-1) != '|') {
        *bg_pos = '\0';
        cmd->background = 1;
    }
    
    char *redirect_pos = strchr(work_str, '>');
    if (redirect_pos) {
        if (strchr(redirect_pos + 1, '>')) {
            free(work_str);
            print_error();
            return;
        }
        
        *redirect_pos = '\0';
        redirect_pos++;
        
        while (*redirect_pos == ' ' || *redirect_pos == '\t') {
            redirect_pos++;
        }
        
        if (*redirect_pos == '\0') {
            free(work_str);
            print_error();
            return;
        }
        
        char *filename_end = redirect_pos;
        while (*filename_end && *filename_end != ' ' && *filename_end != '\t' && 
               *filename_end != '&' && *filename_end != '|' && *filename_end != ';') {
            filename_end++;
        }
        
        if (filename_end > redirect_pos) {
            size_t len = filename_end - redirect_pos;
            cmd->output_file = malloc(len + 1);
            if (cmd->output_file) {
                strncpy(cmd->output_file, redirect_pos, len);
                cmd->output_file[len] = '\0';
            }
        }
    }
    
    char *trimmed = trim_whitespace(work_str);
    if (strlen(trimmed) == 0) {
        free(work_str);
        return;
    }
    
    parse_simple_command(shell, trimmed, cmd);
    
    free(work_str);
}

void parse_simple_command(Shell *shell, char *cmd_str, Command *cmd) {
    char *str = strdup(cmd_str);
    char *saveptr;
    
    if (!str) return;
    
    char *aliased_cmd = resolve_alias(shell, str);
    if (aliased_cmd) {
        free(str);
        parse_command(shell, aliased_cmd, cmd);
        return;
    }
    
    int arg_count = 0;
    char *arg = strtok_r(str, " \t", &saveptr);
    
    while (arg && arg_count < MAX_ARGS - 1) {
        char arg_copy[SHELL_MAX_INPUT];
        strncpy(arg_copy, arg, sizeof(arg_copy) - 1);
        arg_copy[sizeof(arg_copy) - 1] = '\0';
        expand_variables(shell, arg_copy);
        
        cmd->args[arg_count] = strdup(arg_copy);
        arg_count++;
        arg = strtok_r(NULL, " \t", &saveptr);
    }
    
    cmd->arg_count = arg_count;
    free(str);
}

void expand_variables(Shell *shell, char *str) {
    char result[SHELL_MAX_INPUT] = {0};
    char *src = str;
    char *dst = result;
    
    while (*src && (dst - result) < SHELL_MAX_INPUT - 1) {
        if (*src == '$' && *(src+1)) {
            if (*(src+1) == '?') {
                char exit_status[32];
                snprintf(exit_status, sizeof(exit_status), "%d", shell->exit_status);
                size_t len = strlen(exit_status);
                if ((dst - result) + len < SHELL_MAX_INPUT - 1) {
                    strcat(result, exit_status);
                    src += 2;
                    dst = result + strlen(result);
                } else {
                    *dst++ = *src++;
                    *dst = '\0';
                }
                continue;
            } else if (*(src+1) == '$') {
                char pid_str[32];
                snprintf(pid_str, sizeof(pid_str), "%d", shell->shell_pid);
                size_t len = strlen(pid_str);
                if ((dst - result) + len < SHELL_MAX_INPUT - 1) {
                    strcat(result, pid_str);
                    src += 2;
                    dst = result + strlen(result);
                } else {
                    *dst++ = *src++;
                    *dst = '\0';
                }
                continue;
            }
            
            char *var_start = src + 1;
            char *var_end = var_start;
            while (*var_end && (isalnum((unsigned char)*var_end) || *var_end == '_')) {
                var_end++;
            }
            
            if (var_end > var_start) {
                char var_name[SHELL_MAX_INPUT];
                size_t var_len = var_end - var_start;
                if (var_len >= SHELL_MAX_INPUT - 1) var_len = SHELL_MAX_INPUT - 2;
                strncpy(var_name, var_start, var_len);
                var_name[var_len] = '\0';
                
                char *value = get_env_var(shell, var_name);
                if (value) {
                    size_t value_len = strlen(value);
                    if ((dst - result) + value_len < SHELL_MAX_INPUT - 1) {
                        strcat(result, value);
                        dst = result + strlen(result);
                    }
                }
                src = var_end;
            } else {
                *dst++ = *src++;
                *dst = '\0';
            }
        } else {
            *dst++ = *src++;
            *dst = '\0';
        }
    }
    
    if ((dst - result) < SHELL_MAX_INPUT) {
        *dst = '\0';
    } else {
        result[SHELL_MAX_INPUT - 1] = '\0';
    }
    
    strncpy(str, result, SHELL_MAX_INPUT - 1);
    str[SHELL_MAX_INPUT - 1] = '\0';
}

char *trim_whitespace(char *str) {
    if (!str) return str;
    
    char *end;
    
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == 0) return str;
    
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    *(end + 1) = '\0';
    return str;
}