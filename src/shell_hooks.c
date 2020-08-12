#include "headers/shell_hooks.h"

void init_hooks(size_t len, cmd hooks[len]) {
    size_t i;
    for( i = 0; i<len;i++ ) {
        shell_hook(hooks[i]);
    }
}

int show_help() {
    shell_help_menu();
    return 0;
}

int exit_shell() {
    return SHELL_EXIT;
}