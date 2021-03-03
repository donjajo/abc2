#include "headers/utils.h"

inline void error_terminate( const char* fn, const char* err ) {
    perror(err);
    fprintf( stderr, "%s\n", fn);
    exit(EXIT_FAILURE);
}

inline void* __memcpy(void* dest, void* src, size_t len ) {
    for( size_t i = 0; i<len; i++, dest++, src++ ) {
        *((unsigned*)dest) = *((unsigned*)src);
    }

    return dest;
}

inline int findunit( int num, int sum ) {
    int x = num/( num < 0 ? -10 : 10);
    if ( num < 0 ) {
        sum++;
    }

    if ( x < 1 ) {
        return sum;
    }
    sum++;
    return findunit(x, sum);
}

void* memrz(void* ptr, size_t total_allocated, size_t total_used ) {
    if ( total_allocated > total_used ) {
        ptr = realloc(ptr, total_used);
        if ( !ptr ) {
            perror( "realloc" );
            exit(EXIT_FAILURE);
        }
    }
    
    return ptr;
}

void* memincr(void* restrict ptr, size_t used, size_t towrite, size_t* total, size_t incr) {
    if ( used+towrite > *total ) {
        *total += incr;
        while( *total < (used+towrite) ) {
            *total += towrite;
        }
        ptr = realloc(ptr, *total);
        if ( !ptr ) {
            perror( "realloc" );
            exit(EXIT_FAILURE);
        }
    }

    return ptr;
}

_Bool is_rel_path(char const* path) {
    return !is_abs_path(path);
}

_Bool is_abs_path(char const* path) {
    return startswith( "/", path);
}

char* getpath(char const* filename) {
    if ( !filename )
        return 0;

    char* abspath;    
    char* path = strrchr_r(filename, '/');

    if ( !path ) {
        abspath = getcwd(0, 0);
        return abspath;
    } else if ( is_rel_path(path) ) {
        abspath = getcwd(0, 0);
        size_t newlen = strlen(abspath)+1+strlen(path)+1;
        char* newpath = malloc(sizeof(char[newlen]));
        if ( !newpath ) {
            error_terminate(__func__, "malloc");
        }
        strcpy(newpath, abspath);
        strcat(newpath, "/");
        strcat(newpath, path);
        newpath[newlen-1] = '\0';

        free(abspath);
        free(path);
        
        return newpath;
    }

    return path;
}

_Bool confirm(char const* msg, char const* yes, char const* no ) {
    if ( !msg || !yes || !no ) 
        return 0;

    size_t sz = strlen(yes)+strlen(no);
    char buf[sz];
    memset(buf, 0, sizeof(char[sz]));

    while( strcmp(buf, yes) != 0 && strcmp(buf, no) != 0 ) {
        printf( msg );
        fflush(stdout);
        scanf( "%s", buf);
    }

    return !strcmp(buf, yes);
}