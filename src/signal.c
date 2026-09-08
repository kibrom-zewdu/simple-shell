#include "shell.h"

void setup_signal_handlers(void) {
    struct sigaction sa;
    
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    
    sa.sa_handler = (void (*)(int))wait_for_background;
    sigaction(SIGCHLD, &sa, NULL);
}