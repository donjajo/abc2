#ifndef INC_STRINGS_H
    #define INC_STRINGS_H 1
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
    #include <wchar.h>
    #include <stdlib.h>
    #include "utils.h"
    
    #ifndef ABC2_STR_RESET
        #define ABC2_STR_RESET  1
    #endif 

    /**
     * Strips out left side character if present in string
     * 
     * @param char *str  String to trim
     * @param char c    Character to strip off
     * 
     * @return char *p New trimmed string
     * */
    char *ltrim( char* str, const char c );

    /**
     * Strips out righ side character if present in string
     * 
     * @param char *str  String to trim
     * @param char c    Character to strip off
     * 
     * @return char *p New trimmed string
     * */
    char *rtrim( char* str, const char c, int ntimes );
    _Bool unclosedquote(char* str);
    _Bool is_num( size_t len, wchar_t str[len]);
    wchar_t* convert_to_human( wchar_t c, wchar_t* buf );
    size_t convert_to_wchar( char const* chars, wchar_t** buf );
    char* readline(char const* str, int state);
    _Bool startswith(char const* needle, char const* haystack);
    char* strrchr_r(char const* haystack, char needle);
#endif