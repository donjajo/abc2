#ifndef ABC2_PARSER_H
    #define ABC2_PARSER_H 1

    #include <stdio.h>
    #include "types.h"
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <sys/stat.h>
    #include "strings.h"
    #include "utils.h"

    #define MAX_READ_BUF        1024
    #define ABC2_P_COMMENTCHAR  '#'
    #define ABC2_P_ESCCHAR      '\\'
    #define ABC2_P_DELIM        '\t'
    #define ABC2_P_E_NOKEY      -1
    #define ABC2_P_E_ISCOMMENT  0
    #define ABC2_P_E_NOLINE     -2
    #define ABC2_P_KEYS_LEN     96
    #define ABC2_P_KEYS_LEN_INC     16
    #define ABC2_P_MAP_COUNT    164
    #define ABC2_P_MAP_COUNT_INC    16

    struct keyfile* load_keyfile( char const* filename, size_t* s );
    _Bool is_comment( wchar_t const* wcs );
    static int split_to_keyfile( wchar_t* line, struct keyfile* k, _Bool* escapestat );
    static long split_lines(FILE* f, struct keyfile** keysbuf );

#endif