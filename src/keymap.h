#ifndef KEYMAP_H
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdarg.h>
    #include "strings.h"
    
    #define KEYMAP_H 1
    #define KEY_LEN_CUR 0
    #define REMAP_ERR_CHAROVERFLOW 2
    #define REMAP_ERR_MAP_EXISTS -1
    #define REMAP_ERR_ISNULL -3

    typedef struct key {
        int n;
        size_t len;
        char* maps;
    } key;

    typedef void (*iter_f)(key*, int argc, va_list arg_list );

    _Bool map_num( int num, size_t len, char maps[len] );
    int resize_obj(size_t nsize);
    key* key_exists( int n );
    size_t key_count(size_t c);
    key* get_obj(key* nobj);
    int view_keymaps();
    void destroy_obj();
    int remap_keymap(int n, char const* values );
    key* map_exists( char c );
    _Bool map_one(int n, char c);
    _Bool __iter_obj(iter_f func, int argc, ...);
    void _write_line( key* k, int argc, va_list arglist );
    _Bool export_key(char const* file);
#endif