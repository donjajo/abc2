#ifndef ABC2_H
    #define ABC2_H 1
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <linux/limits.h>
    #include <unistd.h>
    #include <locale.h>
    #include "shell/shell.h"
    #include "src/headers/shell_hooks.h"
    #include "src/headers/keymap.h"
    #include "src/headers/decoder.h"

    int decode( size_t argc, char** argv);
    void resetoptind( int optind[static 1] );
#endif
