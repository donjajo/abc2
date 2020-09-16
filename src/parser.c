#include "headers/parser.h"

struct keyfile* load_keyfile( char const* filename, size_t* s ) {
    FILE* f = fopen(filename, "r");
    struct keyfile* buf = 0;
    struct stat statbuf;
    long len;

    if ( !f ) {
        perror( filename );
        return 0;
    }

    if ( stat(filename, &statbuf) < 0 ) {
        perror( "stat" );
        return 0;
    }

    if ( !S_ISREG(statbuf.st_mode) ) {
        fprintf(stderr, "%s is not a regular file\n", filename);
        return 0;
    } 

    len = split_lines(f, &buf);
    fclose(f);
    
    if ( len > 0 ) {
        *s = (size_t) len; 
        return buf;
        // for( size_t i = 0; i < len; i++ ) {
        //     printf( "%d - ", buf[i].n);
        //     for ( size_t j = 0; j < buf[i].len; j++ ) {
        //         printf( "%lc", buf[i].maps[j]);
        //     }
        //     printf( "\n" );
        // }
    }

    return 0;
}

static long split_lines(FILE* f, struct keyfile** keysbuf ) {
    ssize_t nread;
    char* buf = 0;
    size_t len;
    size_t mlen;
    size_t i = 0;
    size_t linenumber = 0;
    size_t total_count = ABC2_P_KEYS_LEN;

    if ( !f ) 
        return 0;
    
    if ( !*keysbuf ) {
        *keysbuf = malloc(sizeof(struct keyfile[total_count]));
        if ( !*keysbuf ) {
            error_terminate(__func__, "malloc");
        }
    }

    while( ( nread = getline(&buf, &len, f) ) >= 0 ) {
        struct keyfile k = {0,0,0};
        int s;
        linenumber++;

        mlen = mbstowcs(0, buf, 0);
        if ( mlen == (size_t)-1 ) {
            perror( "mbstowcs" );
            return -1;
        }

        wchar_t wbuf[mlen+1];// = malloc(sizeof(wchar_t[mlen+1]));
        if ( mbstowcs(wbuf, buf, mlen) == (size_t)-1) {
            perror( "mbstowcs" );
            return -1;
        }
        wbuf[mlen]=0;

        s = split_to_keyfile(wbuf, &k);

        if ( s < 1 ) {
            switch( s ) {
                case ABC2_P_E_NOKEY:
                    fprintf( stderr, "No key provided in line %ld\n", linenumber );
                    return s;
                case ABC2_P_E_NOLINE:
                    fprintf(stderr, "Empty line\n" );
                    return s;
                case ABC2_P_E_ISCOMMENT:
                    continue;
                default:
                    fprintf(stderr, "An unknown error occurred" );
                    return s;
            }
        }

        if ( i+1 > total_count ) {
            total_count += ABC2_P_KEYS_LEN_INC;
            *keysbuf = realloc(*keysbuf, sizeof( struct keyfile[total_count]));
            if ( !*keysbuf ) {
                error_terminate( __func__, "realloc" );
            }
        }

        // *keysbuf[i]
        *((*keysbuf)+i) = k;
        i++;
    }

    if ( i+1 < total_count ) {
        *keysbuf = realloc( *keysbuf, sizeof(struct keyfile[i+1] ));
        if ( !*keysbuf ) {
            error_terminate( __func__, "realloc" );
        }
    }

    return i;
}

inline _Bool is_comment( wchar_t const* wcs ) {
    return ( wcs && wcs[0] == ABC2_P_COMMENTCHAR );
} 

static inline void key_to_num(size_t i, wchar_t* line, int* dest ) {
    wchar_t kbuf[i+1];
    memcpy(kbuf, line, sizeof(wchar_t[i]));
    kbuf[i] = 0;
    *dest = wcstol(kbuf, 0, 0);
}

static int split_to_keyfile( wchar_t* line, struct keyfile* k ) {
    _Bool passed_delim = 0;
    size_t mapcount = ABC2_P_MAP_COUNT;
    int ret = ABC2_P_E_NOKEY;

    if ( line ) {
        if ( is_comment(line) ) {
            return ABC2_P_E_ISCOMMENT;
        }

        k->maps = malloc(sizeof(wchar_t[mapcount]));
        if ( !k->maps ) {
            error_terminate( __func__, "malloc" );
        }

        for( size_t i = 0; line[i] && line[i] != L'\n'; i++ ) {
            if ( i == 0 && line[i] == ABC2_P_DELIM ) {
                ret = ABC2_P_E_NOKEY;
                break;
            }

            if ( !passed_delim && line[i] == ABC2_P_DELIM ) {
                if ( !is_num(i, line) ) {
                    ret = ABC2_P_E_NOKEY;
                    break;
                }

                key_to_num(i, line, &k->n );
                passed_delim = 1;
                continue;
            }

            if (passed_delim) {
                k->len++;
                if ( k->len > mapcount ) {
                    mapcount += ABC2_P_MAP_COUNT_INC;
                    k->maps = realloc(k->maps, sizeof(wchar_t[mapcount]));
                    if ( !k->maps ) {
                        error_terminate( __func__, "realloc" );
                    }
                }

                k->maps[k->len-1] = line[i];
                ret = 1;
            } 

        }

        // Reshrink memory, dont waste it. We just don't want to realloc a lot inside a loop
        if ( mapcount > k->len ) {
            k->maps = realloc( k->maps, sizeof( wchar_t[k->len] ) );
            if ( !k->maps && k->len ) {
                error_terminate( __func__, "realloc" );
            }
        }

        return ret;
    }

    return ABC2_P_E_NOLINE;
}