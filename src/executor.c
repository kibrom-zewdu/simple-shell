#include "shell.h"

int execute_command(Shell *shell, Command *cmd) {
    if (cmd->arg_count == 0) return 0;
    
    if (is_builtin(cmd->args[0])) {
        return execute_builtin(shell, cmd);
    }
    
    return execute_external(shell, cmd);
}

int execute_external(Shell *shell, Command *cmd) {
    pid_t pid;
    int status;
    int stdout_backup = -1, stderr_backup = -1;
    
    if (cmd->output_file) {
        stdout_backup = dup(STDOUT_FILENO);
        stderr_backup = dup(STDERR_FILENO);
        
        int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            print_error();
            if (stdout_backup != -1) close(stdout_backup);
            if (stderr_backup != -1) close(stderr_backup);
            return 1;
        }
        
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
    
    char *exec_path = find_executable(shell, cmd->args[0]);
    if (!exec_path) {
        if (cmd->output_file) {
            dup2(stdout_backup, STDOUT_FILENO);
            dup2(stderr_backup, STDERR_FILENO);
            close(stdout_backup);
            close(stderr_backup);
        }
        fprintf(stderr, "%s: command not found\n", cmd->args[0]);
        return 127;
    }
    
    if (access(exec_path, X_OK) != 0) {
        if (cmd->output_file) {
            dup2(stdout_backup, STDOUT_FILENO);
            dup2(stderr_backup, STDERR_FILENO);
            close(stdout_backup);
            close(stderr_backup);
        }
        fprintf(stderr, "%s: permission denied\n", cmd->args[0]);
        free(exec_path);
        return 126;
    }
    
    pid = fork();
    if (pid == 0) {
        execve(exec_path, cmd->args, shell->env_vars);
        print_error();
        exit(1);
    } else if (pid > 0) {
        if (!cmd->background) {
            waitpid(pid, &status, 0);
        }
        
        if (cmd->output_file) {
            dup2(stdout_backup, STDOUT_FILENO);
            dup2(stderr_backup, STDERR_FILENO);
            close(stdout_backup);
            close(stderr_backup);
        }
        
        free(exec_path);
        
        if (!cmd->background && WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return 0;
    } else {
        if (cmd->output_file) {
            dup2(stdout_backup, STDOUT_FILENO);
            dup2(stderr_backup, STDERR_FILENO);
            close(stdout_backup);
            close(stderr_backup);
        }
        print_error();
        free(exec_path);
        return 1;
    }
}

void wait_for_background(void) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        (void)status;
    }
}