#ifndef ABC2_H
    #define ABC2_H 1
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <linux/limits.h>
    #include <unistd.h>
    #include "shell/shell.h"
    #include "src/shell_hooks.h"
    #include "src/keymap.h"
    #include "src/decoder.h"

    int decode( size_t argc, char** argv);
#endif
