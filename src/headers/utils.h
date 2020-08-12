#ifndef ABC2_UTILS_H
    #define ABC2_UTILS_H 1

    #include <stdio.h>
    #include <stdlib.h>

    void error_terminate( const char* fn, const char* err );
    void* __memcpy(void* dest, void* src, size_t len );
#endif