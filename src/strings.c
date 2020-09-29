#include "headers/strings.h"

char* ltrim( char* str, const char c ) {
    if( str[0] != c ) 
        return str;

    size_t i;
    size_t len = strlen(str);

    for( i=0; str[i] == c; i++ );

    str = memmove( str, str+i, (len-1)+1 );
    return str;
}

char* rtrim( char* str, const char c, int ntimes ) {
    size_t i = strlen(str)-1;

    if ( (int) i < 1 ) 
        return str;

    ntimes = ntimes == 0 ? i : ntimes;

    if ( str[i] != c ) {
        return str;
    }

    for( ; str[i] == c && ntimes > 0; i--, ntimes-- );
    
    str[i+1] = '\0';

    return str;
}

_Bool unclosedquote(char* str) {
    _Bool s_quoteopened=0;
    _Bool d_quoteopened = 0;
    char singlequote = '\'';
    char doublequote = '"';
    char escape = '\\';
    size_t i;

    for( i=0; str[i]; i++ ) {
        _Bool is_escape = i>0 && str[i-1] == escape;

        if ( str[i] == singlequote && !s_quoteopened && !is_escape && !d_quoteopened ) 
            s_quoteopened = 1;
        else if ( str[i] == doublequote && !d_quoteopened && !is_escape && !s_quoteopened ) 
            d_quoteopened = 1;
        else if ( str[i] == singlequote && !is_escape  )
            s_quoteopened = 0;
        else if ( str[i] == doublequote && !is_escape ) 
            d_quoteopened = 0;
    }

    return s_quoteopened || d_quoteopened;
}

_Bool is_num( size_t len, wchar_t str[len]) {
    size_t i;
    int n;

    if ( len > 0 ) {
        for( i=0;i<len;i++) {
            if ( len > 1 && i == 0 && str[i] == L'-' )
                continue;

            n = str[i] - '0';
            if ( n < 0 || n > 9 ) {
                return 0;
            }
        }
    }

    return 1;
}

wchar_t* convert_to_human( wchar_t c, wchar_t* buf ) {
    switch( c ) {
        case L'\t':
            wcscat(buf, L"\\t");
            break;
        case L'\n':
            wcscat(buf, L"\\n");
            break;
        case L'\b':
            wcscat( buf, L"\\b" );
            break;
        case L' ':
            wcscat(buf, L"̺");
            break;
        default:
            buf[0] = c;
            buf[1] = L'\0';
            break;
    }

    return buf;
}