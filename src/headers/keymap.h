#ifndef KEYMAP_H
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdarg.h>
    #include <assert.h>
    #include <wchar.h>
    #include "utils.h"
    #include "strings.h"
    #include "parser.h"
    #include "types.h"
    
    #define KEYMAP_H 1
    #define KEY_LEN_CUR 0
    #define KEYMAP_DELIMITER L","
    #define REMAP_ERR_CHAROVERFLOW 2
    #define REMAP_ERR_MAP_EXISTS -1
    #define REMAP_ERR_ISNULL -3


    _Bool map_num( int num, size_t len, kv_int maps[len], _Bool ischar );
    int resize_obj(size_t nsize);
    key* key_exists( int n );
    size_t key_count(size_t c);
    key* get_obj(key* nobj);
    int view_keymaps();
    void destroy_obj();
    int remap_keymap(int n, char const* values );
    key* map_exists( kv_int c, _Bool ischar, size_t* index );
    _Bool map_one_int(int n, kv_int c, _Bool fromchar);
    _Bool map_one_char( int n, kv_char c );
    _Bool __iter_obj(iter_f func, int argc, ...);
    void _write_line( key* k, int argc, va_list arglist );
    _Bool export(char const* file);
    void addwchar(key* k );
    _Bool is_char(size_t index, size_t len, size_t indexes[len] );
    _Bool delete_keymaps( char* keymaps );
    _Bool delete( kv_int v, _Bool ischar );
    void delete_wchars( size_t index, key* keymap );
    void decrement_wchars( size_t start, key* keymap );
    _Bool delete_n( int n );
    _Bool map_from_keyfile(struct keyfile key, _Bool ignore_dup );

    #define map_one(N,V) _Generic( V, kv_int: map_one_int, kv_char: map_one_char ) (N,V, 0)
#endif