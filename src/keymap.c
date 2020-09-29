#include "headers/keymap.h"

/**
 * View list of keymaps
 * 
 * @return  int     Exit status
 * 
*/
int view_keymaps() {
    size_t i;
    size_t j;
    size_t c = key_count(KEY_LEN_CUR); // Get count of mapped objects
    key* obj = get_obj((void*)0); // Get the object pointer containing all mappings
    wchar_t buf[3];
    
    if ( !c ) {
        printf( "No maps yet!\n" );
        return 0;
    }
    // Iterate and print all 
    for( i=0;i<c;i++) {
        printf( "%d: {", obj[i].n);
        for( j=0; j<obj[i].len;j++) {
            if ( is_char(j, obj[i].wcharcount, obj[i].wchars ) ) {
                memset(buf, 0, sizeof(wchar_t[3]));
                convert_to_human((kv_char) obj[i].maps[j], buf);
                printf( "'%ls'",  buf);
            } else 
                printf( "'%Ld'", obj[i].maps[j]);

            printf( "%c", (j+1!=obj[i].len ? ',' : '\0') );
        }
        printf( "}\n");
    }

    return 0;
}

/** 
 * Remap a key, appending a new value
 * 
 * @param   int     n       Number to map
 * @param   char*   values  Comma separated characters
 * 
 * @return  int     ret     Return status
 * 
*/
int remap_keymap(int n, char const* values ) {
    if ( !values )
        return REMAP_ERR_ISNULL;

    // const wchar_t del[2] = L",";
    wchar_t* state;
    size_t sz = mbstowcs(0, values, 0);
    
    wchar_t* t = calloc(sz+1, sizeof(wchar_t)); // Helps us zero out the values
    if ( !t ) {
        error_terminate( __func__, "calloc" );
    }
    mbstowcs(t, values, sz);
    
    wchar_t* tokbuf = wcstok( t, KEYMAP_DELIMITER, &state);
    int ret = 0;

    for( ; tokbuf; tokbuf = wcstok( NULL, KEYMAP_DELIMITER, &state ) ) {
        // tokbuf = ltrim(tokbuf, ' ');
        // tokbuf = rtrim(tokbuf, ' ', 0);
        size_t len = wcslen(tokbuf);
        
        _Bool isnum = is_num(len, tokbuf);
        key* k;

        kv_int v = isnum ? wcstoll(tokbuf, 0, 0) : tokbuf[0];
        
        if ( len > 1 && !isnum ) {
            ret = REMAP_ERR_CHAROVERFLOW;
            break; // I know, I would have just returned here. But I need to free(t) and I want to write it once
        }

        if ( len == 0 ) { // Because we stripped out spaces. It is mostly likey to be 0.  So, a space character was provided
            tokbuf[0]=' ';
        }
        
        k = map_exists(v, !isnum, 0);
        if ( k ) {
            ret = REMAP_ERR_MAP_EXISTS;
            break;
        }

        if ( /* k == NULL || (k != NULL && k->n != n )*/ 1 ) {
            // printf( "%ls\n", tokbuf);
            if ( isnum ) {
                map_one_int(n, v, 0);
            } else {
                map_one_char(n, (kv_char)v);
            }
        }
    }

    free(t);    

    return ret;
}


size_t split_keymaps( char* keymaps, wchar_t*** buf ) {
    size_t mbslen = mbstowcs( 0, keymaps, 0 );
    wchar_t** b;
    wchar_t* ptr;
    wchar_t* tokbuf;
    wchar_t* wcharbuf = calloc(mbslen+1, sizeof(wchar_t));
    size_t i = 0;

    if ( !wcharbuf ) {
        error_terminate( __func__, "calloc" );
    }

    if ( mbstowcs(wcharbuf, keymaps, mbslen) == (size_t)-1 ) { // Convert multibyte to wide characters
        error_terminate(__func__, "mbstowcs" );
    }

    if (!*buf) {
        *buf = malloc(sizeof(wchar_t*[1]));
        if ( !*buf ) {
            error_terminate(__func__, "malloc" );
        }
    }

    tokbuf = wcstok(wcharbuf, KEYMAP_DELIMITER, &ptr);

    for( ; tokbuf; tokbuf = wcstok(0, KEYMAP_DELIMITER, &ptr), i++ ) {
        if ( i > 0 ) {
            *buf = realloc(*buf, sizeof(wchar_t*[i+1]));
            if ( !*buf ) {
                error_terminate( __func__, "realloc" );
            }
        }

        b = *buf;
        b[i] = malloc(sizeof(wchar_t[wcslen(tokbuf)+1]));
        wcscpy(b[i], tokbuf);
    }  

    free(wcharbuf);

    return i;
}

_Bool delete_keymaps( char* keymaps ) {
    wchar_t** buf = 0;
    size_t maps_count = split_keymaps(keymaps, &buf);
    kv_int val;

    if ( maps_count ) {
        for( size_t i = 0; i < maps_count; i++ ) {
            size_t len = wcslen(buf[i]);
            _Bool isnum = is_num(1, buf[i]);
            
            if ( len == 1 ) {                
                val = isnum ? wcstoull(buf[i], 0, 0) : (kv_int)buf[i][0];
                delete(val, !isnum);
            }
            free(buf[i]);
        }

        free(buf);
        return 1;
    }

    return 0;
}

/**
 * Delete a key map 
 * 
 * @param   int     Mapped number
 * @param   kv_char Value to delete
 * 
 * @return  _Bool
*/
_Bool delete( kv_int v, _Bool ischar ) {
    size_t index;
    key* k = map_exists( v, ischar, &index );

    if ( k ) {
        for( size_t i = 0; i<k->len; i++ ) {
            if ( k->maps[i] == v ) {
                
                if ( ischar && !is_char( i, k->wcharcount, k->wchars ) ) {
                    continue;
                }

                size_t offset = k->len - (i+1);

                if ( i+1 >= k->len ) {
                    memmove(k->maps, k->maps, sizeof(kv_int[i]));
                } else {
                    memmove(k->maps+i, k->maps+(i+1), sizeof(kv_int[offset]));
                }
                k->len--;

                k->maps = realloc(k->maps, sizeof(kv_int[k->len]));
                if ( !k->maps && k->len > 0 ) {
                    error_terminate( __func__, "realloc" );
                }

                if ( ischar ) {
                    delete_wchars(i, k);
                }

                if ( k->len <= 0 ) { // All keymaps have been removed. Everything related to this key is gone, then remove finally
                    delete_n(k->n);
                }
                return 1;
            }
        }
        return 1;
    }

    return 0;
}

void delete_wchars( size_t index, key* keymap ) {
    size_t n;

    for( size_t i = 0; i<keymap->wcharcount; i++ ) {
        if ( keymap->wchars[i] == index ) {
            if ( i+1 >= keymap->wcharcount ) { // If element is the last one. Copy all excep the last one
                keymap->wchars = memmove(keymap->wchars, keymap->wchars, sizeof( size_t[keymap->wcharcount-1]) ); // Copy all except the last one
            } else {
                n = keymap->wcharcount - (i+1); // Get number of bytes offset t    o copy 
                memmove(&keymap->wchars[i], &keymap->wchars[i+1], sizeof(size_t[n]));
            }

            keymap->wcharcount--; // Work is done. Decrement count
            keymap->wchars = realloc(keymap->wchars, sizeof(size_t[keymap->wcharcount]));
            if ( !keymap->wchars && keymap->wcharcount > 0 ) {
                error_terminate( __func__, "realloc" );
            }

            decrement_wchars(index, keymap);
            break;
        }
    }
}

inline void decrement_wchars( size_t start, key* keymap ) {
    size_t i;
    for ( i = 0; i < keymap->wcharcount; i++ ) {
        if ( keymap->wchars[i] > start ) {
            keymap->wchars[i]--;
        }
    }
}

/**
 * Deletes a mapped number
 * 
 * @param   int     The number
 * 
 * @return  _Bool   True|False if deleted
 * 
*/
_Bool delete_n( int n ) {
    key* keys = get_obj(0);
    size_t len = key_count(0);
    size_t offset;

    for( size_t i = 0; i<len; i++ ) {
        if (keys[i].n == n ) {
            if ( i+1 >= len ) {
                memmove(keys, keys, sizeof(key[len-1]));
            }
            else {
                offset = len-(i+1);
                memmove(keys+i, keys+(i+1), sizeof(key[offset]));
            }

            len = key_count(-1);
            keys = realloc(keys, sizeof(key[len]));
            if ( !keys && len ) {
                error_terminate(__func__, "realloc" );
            }

            get_obj(keys); // Set new pointer object
            return 1;
        }
    }

    return 0;
}

/**
 * Has only one job. Increment or decrement a static count
 * 
 * @param   size_t     c   Number to sum
 * 
 * @return  size_t     Returns new sum
 * 
*/
inline size_t key_count(size_t c) {
    static size_t i=0;

    i += c;
    return i;
}

key* get_obj(key* nobj) {
    static key* p;
    
    // Got new object? Probably after a realloc. Set to new object
    if ( nobj ) {
        p = nobj;
        return 0;
    }

    if ( !p ) {
        p = malloc(sizeof(key));
        if ( !p ) {
            error_terminate( __func__, "malloc" );
        }
    }

    return p;
}

void destroy_obj() {
    key* obj;

    if ( (obj=get_obj(0)) != NULL ) {
        size_t len = key_count(0);
        
        for( size_t i =0; i<len;i++) {
            key k = obj[i];
            if ( k.maps ) 
                free(k.maps);
            if( k.wchars ) 
                free(k.wchars);
        }
        free(obj);
    }
}

key* key_exists( int n ) {
    key* obj = get_obj((void*)0);
    if ( !obj ) {
        return NULL;
    }

    size_t i;
    size_t len = key_count(KEY_LEN_CUR);

    for( i=0; i<len; i++ ) {
        if (obj[i].n == n) {
            return &obj[i];
        }
    }

    return NULL;
}

_Bool map_from_keyfile(struct keyfile key, _Bool ignore_dup ) {
    for( size_t i = 0; i < key.len; i++ ) {
        kv_int value;
        _Bool ischar = 0;
        size_t exists;

        if ( is_num(1, key.maps+i) ) {
            wchar_t b[2] = {key.maps[i], 0};
            value = wcstoull(b, 0, 0);
        } else {
            value = (kv_int) key.maps[i];
            ischar = 1;
        }

        if ( map_exists(value, ischar, &exists) ) { 
            if ( !ignore_dup ) {
                printf( "%d:%lc already exists. Please remove duplicates or pass -i to ignore duplicates\n", key.n, key.maps[i] );
                return 0;
            } else {
                continue;
            }
        } 

        map_one_int(key.n, value, ischar); 
    }

    return 1;
}

_Bool map_one_char( int n, kv_char c ) {
    return map_one_int(n, (kv_int)c, 1);
}

_Bool map_one_int(int n, kv_int c, _Bool fromchar ) {
    key* k = key_exists(n);

    if ( k ) {
        k->maps = realloc( k->maps, sizeof(kv_int[k->len+1]) );
        if (!k->maps) {
            error_terminate(__func__, "realloc" );
            return 0;
        }

        k->maps[k->len] = c;
        k->len++;

        if ( fromchar ) {
            addwchar( k );
        }

        return 1;
    } else {
        kv_int* a = malloc(sizeof(kv_int[1]));
        a[0] = c;
        
        return map_num(n, 1, a, fromchar);
    }
}

_Bool map_num( int num, size_t len, kv_int maps[len], _Bool ischar ) {
    key* k;
    key* obj;
    size_t count = key_count(KEY_LEN_CUR);
    key newkey = (key) {.len=len, .n=num, .maps=maps, .wchars=0, .wcharcount=0};
    
    // Count == 0? Then get_obj() has malloc of sizeof(key) already. Go ahead and set the first key
    if ( count == 0 ) {
        obj = get_obj(NULL);
        obj[0] = newkey;

        if (ischar)
            addwchar(&obj[0]);

        key_count(+1); // One key set
        return 1;
    }

    if ( (k = key_exists(num)) == NULL ) {
        // Resize to allocate more space. Tricky part, count = 1, adding new element, we want to resize to count+1 then set for index=count (index starts at 0)
        if ( resize_obj(count+1) ) {
            obj = get_obj((void*)0);
            obj[count] = newkey;

            if (ischar)
                addwchar(&obj[count]);

            key_count(+1); // Key is set, increase count
        } 
        return 0;
    } else {
        k->maps = maps;
        k->len = len;
        if ( ischar ) 
            addwchar(k);
    }

    return 1;
}

void addwchar(key* k ) {
    if ( k->wcharcount < 1 ) {
        k->wchars = malloc(sizeof(size_t[1]));
        if ( !k->wchars ) {
            error_terminate(__func__, "malloc");
        }

        k->wchars[0] = k->len-1;
    } else {
        k->wchars = realloc( k->wchars, sizeof(size_t[k->wcharcount+1]));
        if ( !k->wchars ) {
           error_terminate( __func__, "realloc");
        }

        k->wchars[k->wcharcount] = k->len-1;
    }

    k->wcharcount++;

    // printf( "Wchars: %ld\n", k->wcharcount );
    // for( size_t i = 0; i<k->wcharcount; i++ ) {
    //     printf( "%ld\n", k->wchars[i]);
    // }
    // printf( "\n" );
}

int resize_obj(size_t nsize) {
    key* obj = get_obj((void*)0);
    size_t osize = key_count(KEY_LEN_CUR);

    if ( nsize > osize ) {
        obj = realloc(obj, sizeof( key[nsize] ) );
        if ( !obj ) {
            perror( "realloc:" );
            return 0;
        }
        
        // New object created, pass to replace
        get_obj(obj);
        return 1;
    }

    return -1;
}

key* map_exists( kv_int c, _Bool ischar, size_t* index ) {
    key* obj = get_obj(0);
    size_t s = key_count(KEY_LEN_CUR);
    size_t i;
    size_t j;

    for(i=0;i<s;i++) {
        for(j=0;j<obj[i].len;j++) {
            if ( !ischar && obj[i].maps[j]==c )  {
                if ( index )
                    *index = i;
                return &obj[i];
            } else if ( ischar && is_char(j, obj[i].wcharcount, obj[i].wchars) && obj[i].maps[j] == c ) {
                if ( index )
                    *index = i;
                return &obj[i];
            }
        }
    }

    return NULL;
}


_Bool is_char(size_t index, size_t len, size_t indexes[len] ) {
    size_t i;
    for( i = 0; i < len; i++ ) {
        if ( index == indexes[i])
            return 1;
    }

    return 0;
}

_Bool __iter_obj(iter_f func, int argc, ...) {
    size_t s = key_count(KEY_LEN_CUR);
    if ( !s ) 
        return 0;

    key* keys = get_obj(NULL);
    size_t i;
    va_list arg_list;
    va_start(arg_list, argc);

    for(i=0;i<s;i++) {
        func(keys+i, argc, arg_list);
    }

    va_end(arg_list);

    return 1;
}

void _write_line( key* k, int argc, va_list arglist ) {
    if ( argc < 1 )
        return;
    
    int digitcount = findunit(k->n, 1);
    size_t len = (k->len + digitcount + 3); // This will produce a line like this "48\tabceowpsi\n"
    wchar_t buf[len];
    static int fd = -1;

    if ( fd == -1 )
        fd = va_arg(arglist, int);

    swprintf(buf, len, L"%d\t", k->n);

    for( int i = 0; i<k->len; i++ ) {
        wchar_t v = (wchar_t) k->maps[i];
        if ( !is_char(i, k->wcharcount, k->wchars) ) {
            wchar_t b[2];
            swprintf(b, 2, L"%Ld", k->maps[i]);
            v = b[0];
        }

        buf[digitcount+(i+1)] = v;
    }
    buf[len-2]=L'\n';
    buf[len-1]=0;

    dprintf( fd, "%ls", buf);
}

_Bool export(char const* filename) {
    int fd;
    size_t len = strlen(filename);
    mode_t perm = S_IRUSR|S_IWUSR;
    mode_t flags = O_WRONLY|O_TRUNC|O_CREAT;

    if ( len ) {
        fd = open( filename, flags, perm);
        if ( fd < 0 ) {
            error_terminate(__func__, "open");
        }
    } else {
        fd = STDOUT_FILENO;
    }

    if ( !len ) {
        printf( "\n\n" );
    }
    dprintf(fd, "#####\tKEY FILE BEGINS\t#####\n" );

    __iter_obj(_write_line, 1, fd);

    dprintf( fd, "#####\tKEY FILE ENDS\t#####" );

    if ( !len ) {
        printf( "\n\n" );
    }

    if ( len )
        close(fd);
    return 1;
    
}