#ifndef ABC2_TYPES_H
    #define ABC2_TYPES_H 1

    #include <stdio.h>
    #include <wchar.h>

    typedef long long kv_int;
    typedef wchar_t kv_char;

    typedef struct key {
        int n;
        size_t len;
        kv_int* maps;
        size_t* wchars;
        size_t wcharcount;
    } key;

    typedef void (*iter_f)(key*, int argc, va_list arg_list );
#endif