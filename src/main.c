#include "shell.h"

int main(int argc, char **argv, char **envp) {
    Shell shell;
    
    init_shell(&shell, argc, argv, envp);
    
    char arg_name[32];
    for (int i = 0; i < argc; i++) {
        snprintf(arg_name, sizeof(arg_name), "%d", i);
        set_env_var(&shell, arg_name, argv[i]);
    }
    
    run_shell(&shell);
    cleanup_shell(&shell);
    
    return shell.exit_status == -1 ? 0 : shell.exit_status;
}