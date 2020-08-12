#ifndef INC_STRINGS_H
    #define INC_STRINGS_H 1
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
    #include <wchar.h>

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
#endif