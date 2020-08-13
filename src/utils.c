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