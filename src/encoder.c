#include "headers/encoder.h"

static inline size_t _line_encode(char* line, wchar_t** retbuf, _Bool ismanaged, size_t* bytes_encoded, size_t* memused, _Bool create_keymaps );

size_t encode_file( FILE* file, wchar_t** retbuf, _Bool create_keymaps ) {
    size_t maxlen = 2096;
    char buf[maxlen];
    _Bool ismanaged = (!*retbuf);
    size_t memused = sizeof(wchar_t[ABC2_EN_BUFMAX]), total_encoded = 0;

    if ( ismanaged ) {
        *retbuf = calloc(memused/sizeof(wchar_t), sizeof(wchar_t));
        if( !*retbuf ) {
            error_terminate(__func__, "calloc");
        }
    }

    while( fgets(buf, maxlen, file) ) {
        size_t l = _line_encode(buf, retbuf, ismanaged, &total_encoded, &memused, create_keymaps);
        if ( !l ) {
            break;
        }
    }

    if ( ismanaged ) {
        *retbuf = memrz(*retbuf, memused, sizeof(wchar_t[total_encoded+1]));
    }

    return total_encoded;
}


static inline size_t _line_encode(char* line, wchar_t** retbuf, _Bool ismanaged, size_t* bytes_encoded, size_t* memused, _Bool create_keymaps ) {
    wchar_t* encoded = encode(line, create_keymaps);
    if ( encoded ) {
        size_t l = wcslen(encoded);
        if ( ismanaged ) {
            *retbuf = memincr(
                *retbuf, 
                sizeof(wchar_t[*bytes_encoded]), 
                sizeof(wchar_t[l]), 
                memused, 
                sizeof(wchar_t[ABC2_EN_BUFINCR])
            );
        }

        memcpy(
            *retbuf + *bytes_encoded, 
            encoded, 
            sizeof(wchar_t[l])
        );

        *bytes_encoded += l;    
        free(encoded);
        return l;
    }

    return 0;
}

size_t encode_string( char const* str, wchar_t** retbuf, _Bool create_keymaps ) {
    size_t len = strlen(str)+1;
    _Bool ismanaged = (!*retbuf);
    size_t memused = sizeof(wchar_t[ABC2_EN_BUFMAX]);
    char* buf = malloc(sizeof(char[len]));
    memcpy(buf, str, sizeof(char[len]));

    char* line = readline(buf, ABC2_STR_RESET);
    size_t bytes_encoded = 0;

    if ( ismanaged ) {
        *retbuf = calloc(memused/sizeof(wchar_t), sizeof(wchar_t)); // malloc(memused);
        if ( !*retbuf ) {
            error_terminate( __func__, "malloc" );
        }
    }

    while( line ) {
        if ( !_line_encode(line, retbuf, ismanaged, &bytes_encoded, &memused, create_keymaps) ) {
            free(line);
            break;
        }

        free(line);
        line = readline(buf, 0);
    }

    if ( ismanaged ) {
        *retbuf = memrz(
            *retbuf, 
            memused, 
            sizeof(wchar_t[bytes_encoded+1])
        );

        *(*retbuf+bytes_encoded) = L'\0';
    }

    free(buf);

    return bytes_encoded;
}

wchar_t* encode( char const* string, _Bool create_keymaps ) {
    wchar_t* buf = 0;
    size_t len;
    if ( ( len = convert_to_wchar(string, &buf) ) ) {
        size_t format_length = wcslen(ABC2_EN_FORMAT);
        size_t encodedmaxlen = 64;
        size_t totalsize = format_length+encodedmaxlen;
        wchar_t encodedbuf[totalsize+2];
        size_t total_chars_encoded = 0;
        size_t total_mem_alloc = sizeof(wchar_t[ABC2_EN_LINEMAX]);
        wchar_t* linebuf = malloc(total_mem_alloc);
        if ( !linebuf ) {
            error_terminate( __func__, "malloc" );
        }
        memset(linebuf, L'\0', total_mem_alloc);

        for( size_t i = 0; buf[i]; i++ ) {
            _Bool ischar = 1;
            kv_int value;

            wchar_t b[2] = {buf[i], L'\0'};
            if ( is_num(1, b ) ) {
                value = wcstoull(b,0, 0);
                ischar = 0;
            } else {
                value = (kv_int)buf[i];
            }
            
            memset(encodedbuf, 0, sizeof(wchar_t[totalsize+1]));
            if ( !getencode(value, ischar, totalsize, encodedbuf) ) {
                if ( create_keymaps ) {
                    int n = generate_rand(0,i);
                    
                    map_one_int(n, value, ischar);
                    getencode(value, ischar, totalsize, encodedbuf);
                } else {
                    printf( "Keymap for %lc does not exist\n", buf[i]);
                    free(linebuf);
                    free(buf);
                    return 0;
                }
            }

        
            size_t encoded_size = wcslen(encodedbuf)+1;
            linebuf = memincr(linebuf, sizeof(wchar_t[total_chars_encoded]), sizeof(wchar_t[encoded_size]), &total_mem_alloc, sizeof(wchar_t[ABC2_EN_LINEINC]));
            wcsncat(linebuf, encodedbuf, totalsize);
            total_chars_encoded += wcslen(linebuf);
        }

        // if ( total_mem_alloc > total_chars_encoded ) {
        //     linebuf = realloc(linebuf, sizeof(wchar_t[total_chars_encoded+1]));
        //     if ( !linebuf ) {
        //         error_terminate( __func__, "realloc");
        //     }
        // }

        linebuf = memrz(linebuf, total_mem_alloc, sizeof(wchar_t[total_chars_encoded+1]) );

        free(buf);
        return linebuf;
    }

    return 0;
}

_Bool getencode(kv_int value, _Bool ischar, size_t n, wchar_t writebuf[n]) {
    size_t index;

    key* k = map_exists(value, ischar, &index);

    if ( !k ) 
        return 0;
    
    // size_t len = findunit(k->n, 1) + findunit(index, 1); // Find the length of the 2 integers when counted as characters
    // size_t format_len = wcslen(ABC2_EN_FORMAT); // Get the length of the format
    // size_t total_len = len+format_len; // Add the format length and the integer length to get enough memory. The length of '%d' != the length of 2000

    // wchar_t* buf = malloc(sizeof(wchar_t[total_len+1]));
    // if ( !buf ) {
    //     error_terminate(__func__, "malloc");
    // }
    swprintf(writebuf, n, ABC2_EN_FORMAT, k->n, index);
    writebuf[wcslen(writebuf)] = L'\t'; 

    return 1;
}

int generate_rand(int max, int seed) {
    srandom(seed*time(0));
    max = max == 0 ? ABC2_EN_DEFAULT_RANDMAX : max;
    return (max*random()/RAND_MAX+1);
}