#ifndef ABC2_DECODER_H
    #define ABC2_DECODER_H 1
    #define ABC2_DELIMITER ':'
    #define ABC2_PARSE_NULL -1
    #define ABC2_ERR_NOKEY -1
    #define ABC2_ERR_NOMAP -2
    #define ABC2_ERR_PARSE -3
    #define ABC2_ERR_ISNULL -4

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "keymap.h"

    typedef struct abc2parsed {
        int n;
        size_t pos;
    } abc2parsed;

    extern size_t parsecount;
    extern int decodeerr;
    extern abc2parsed decodeerrdata;

    int char2int(char c);
    abc2parsed* abc2_parse(const char* v);
    int ctoi(size_t len, char c[len]);
    _Bool validint(int n);
    _Bool posnull(size_t pos);
    void resetpos(size_t* a);
    _Bool isdelimiter(char c);
    _Bool isnum(char c);
    _Bool isnewline(const char c);
    void printerror( size_t line, size_t col, size_t pos, const char* c, const char* msg );
    char* abc2_decode( char* data );
    void parsedbuf( size_t len, char c, char** buf );
    void reseterr();
#endif