#ifndef ABC2_H
    #define ABC2_H 1
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <linux/limits.h>
    #include <unistd.h>
    #include <locale.h>
    #include <libgen.h>
    #include "shell/shell.h"
    #include "src/headers/types.h"
    #include "src/headers/shell_hooks.h"
    #include "src/headers/keymap.h"
    #include "src/headers/decoder.h"
    #include "src/headers/parser.h"
    #include "src/headers/encoder.h"

    #ifndef ABC2_DEFAULT_EXT
        #define ABC2_DEFAULT_EXT    ".abc"
    #endif 
    
    int decode_file( size_t argc, char** argv);
    void resetoptind( int optind[static 1] );
    char* createcopy( char const* src );
    _Bool keyfile_load(char const* filename, _Bool ignore_dup);
#endif
