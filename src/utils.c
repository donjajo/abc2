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