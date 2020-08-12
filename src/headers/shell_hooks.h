#ifndef SHELL_HOOKS_H
    #define SHELL_HOOKS_H 1
    
    #include <stdio.h>
    #include "../../shell/shell.h"
    
    void init_hooks(size_t len, cmd hooks[len]);
    int show_help();
    int exit_shell();
#endif