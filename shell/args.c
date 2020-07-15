#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "args.h"

_Bool issinglequote( const char c ) {
    return c == '\'';
}

_Bool isdoublequote( const char c) {
    return c == '"';
}

char** shell_split_arg( char* cm, size_t* argc) {
    size_t i=0;
    size_t j=0;
    size_t start=0;
    size_t stop = 0;
    _Bool quoteopened = 0;
    char* tmp = cm;
    char** argv_buf = malloc(sizeof(char*));

    if ( !argv_buf ) {
        fprintf(stderr, __func__ );
        perror( ": malloc:" );
        exit(EXIT_FAILURE);
    }


    for( ; ; i++ ) {
        // If we encounter a quote
        if ( issinglequote(cm[i]) || isdoublequote(cm[i]) ) {
            // Quote not opened? This is a new quote then. Lets mark our start there
            if ( !quoteopened ) {
                start = i+1;
                quoteopened=1;
                continue;
            } else {
                // Quote was already opened? This is a closing then. Lets keep the closing here
                quoteopened = 0;
                stop=i;
            }
        }

        // Is this space? And no stop has already been initialized before by quotes? Stop here
        if ( ( !stop && isspace(cm[i]) && !quoteopened ) || cm[i] == '\0' ) {
            stop=i;
        }

        if ( stop ) {
            size_t s = (stop-start); // Find total characters
            // If j (argc) > 0 then we reallocate more
            if (j) {
                argv_buf = realloc(argv_buf, sizeof(char*)*(j+1));
                if (!argv_buf) {
                    fprintf(stderr, __func__ );
                    perror( ": realloc:" );
                    exit(EXIT_FAILURE);
                }
            }
            argv_buf[j] = malloc(sizeof(char[s+1]));
            
            if ( !argv_buf ) {
                fprintf(stderr, "%s: argv_buf[%ld]:", __func__, j);
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            memcpy(argv_buf[j], tmp+start, s ); // Copy the memory from the start till the length
            argv_buf[j][s]='\0'; // dont forget our ending character for strings
            j++;

            // Make sure this is not the end of the string?
            if (cm[i]!='\0') {
                start = stop+1; // Set the start pointer from where we stopped last
                while(isspace(cm[start])) { // It has been spaces all through? Keep going till we find a valid character, as long as its not the end yet
                    start++;
                }
                i=start-1; // We have done the counting in the above while loop, set it to our counter
            }
        }

        stop = 0;
        if ( cm[i] == '\0' ) 
            break;
    }

    *argc = j;

    return argv_buf;
}