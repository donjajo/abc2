#ifndef ABC2_UTILS_H
    #define ABC2_UTILS_H 1

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include "strings.h"

    void error_terminate( const char* fn, const char* err );
    void* __memcpy(void* dest, void* src, size_t len );
    int findunit( int num, int sum );
    void* memrz(void* ptr, size_t total_allocated, size_t total_used );
    void* memincr(void* restrict ptr, size_t used, size_t towrite, size_t* total, size_t incr);
    _Bool is_rel_path(char const* path);
    _Bool is_abs_path(char const* path);
    char* getpath(char const* filename);
#endif