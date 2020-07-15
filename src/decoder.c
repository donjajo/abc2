#include "decoder.h"

size_t parsecount = 0;
int decodeerr = 0;
abc2parsed decodeerrdata = {0, 0};

abc2parsed* abc2_parse(const char* v) {
    size_t i = 0;
    size_t j = 1;
    size_t k = 1;
    size_t l = 0;
    _Bool iswaiting = 0;
    int n;
    size_t head = ABC2_PARSE_NULL;
    size_t tail = ABC2_PARSE_NULL;
    char* keymap = NULL;
    char* section = NULL;
    const char* t = v;
    reseterr();

    abc2parsed* parsed = malloc(sizeof(abc2parsed));

    if ( !parsed ) {
        printf( "%s:", __func__ );
        perror( "malloc" );
        exit(EXIT_FAILURE);
    }

    for( ; ; i++, k++ ) {

        if ( isnewline(v[i]) ) {
            k=1;
            j++;
        }

        n = char2int(v[i]);
        if ( validint(n) ) {
            if ( posnull(head) )
                head = i;
            continue;
        } else if ( !posnull(head) ) {
            tail = i;
        } else if ( isdelimiter(v[i] ) && posnull(head) ) {
            printerror(j, k, i,v, "Expected a number");
            break;
        } else if ( iswaiting && posnull(head) ) {
            printerror(j, k, i,v, "Expected a number");
            break;
        } else if( !posnull(head) && !iswaiting ) {
            printerror(j, k, i,v, "Expected a number");
            break;
        } 


        if ( !posnull(head) && !posnull(tail) ) {
            size_t offset = tail-head;

            if ( iswaiting ) {
                section = malloc(sizeof(char[offset+1]));
                if ( !section ) {
                    perror( __func__ );
                    exit(EXIT_FAILURE);
                }
                memset(section, 0, offset+1);
                memcpy(section, t+head, offset);
                iswaiting=0;
            } else {
                keymap = malloc(sizeof(char[offset+1]));
                if ( !keymap ) {
                    perror( __func__ );
                    exit(EXIT_FAILURE);
                }
                memset(keymap, 0, offset+1);
                memcpy(keymap, t+head, offset);
            }

            resetpos(&head);
            resetpos(&tail);

            if ( keymap && section ) {
                abc2parsed p = { atoi(keymap), atoi(section) };
                if ( l > 0 ) {
                    parsed = realloc(parsed, sizeof(abc2parsed[l+1]));
                    if ( !parsed ) {
                        printf( "%s", __func__ );
                        perror( "realloc");
                        exit(EXIT_FAILURE);
                    }
                }

                parsed[l] = p;
                l++;

                free(keymap);
                free(section);
                keymap = NULL;
                section = NULL;
            }
        }

        if ( isdelimiter(v[i]) ) {
            iswaiting = 1;
        }

        if ( !v[i] ) {
            break;
        }
    }

    parsecount = l;
    if ( l == 0 ) {
        free(parsed);
        return (void*)0;
    }

    return parsed;
}

char* abc2_decode( char* data ) {
    reseterr();
    if ( data ) {
        abc2parsed* parsed = abc2_parse(data);
        if ( parsed ) {
            size_t i;
            char* buf = (void*)0;

            for( i=0; i<parsecount; i++ ) {
                key* keymap = key_exists(parsed[i].n);
                
                if ( keymap ) {
                    if ( parsed[i].pos > 0 && keymap->len >= parsed[i].pos )
                        parsedbuf(i+1, keymap->maps[parsed[i].pos-1], &buf);
                    else {
                        decodeerr = ABC2_ERR_NOMAP;
                        decodeerrdata = parsed[i];
                        break;
                    }
                } else {
                    decodeerr = ABC2_ERR_NOKEY;
                    decodeerrdata = parsed[i];
                    break;
                }
            }

            free(parsed);

            if ( buf ) {
                buf[i] = '\0';
                return buf;
            }
        } else {
            decodeerr = ABC2_ERR_PARSE;
        }
    } else {
        decodeerr = ABC2_ERR_ISNULL;
    }

    return (void*)0;
}

void parsedbuf( size_t len, char c, char** buf ) {
    if ( !*buf ) {
        *buf = malloc(sizeof(char[len+1]));
        if ( !*buf ) {
            printf( "%s", __func__ );
            perror( "malloc" );
            exit( EXIT_FAILURE );
        }
    } else {
        *buf = realloc(*buf, sizeof(char[len+1]));
        if ( !*buf ) {
            printf( "%s", __func__ );
            perror( "realloc" );
            exit( EXIT_FAILURE );
        }
    }

    *(*buf+(len-1)) = c; 
}

void printerror( size_t line, size_t col, size_t pos, const char* c, const char* msg ) {
    printf( "Syntax Error: Line %ld, Column %ld", line, col);
    if ( msg ) {
        printf( ". %s", msg );
    }
    printf( "\n" );
    size_t i=pos;
    while(c[i]) {
        if (i==pos) {
            printf( "'");
        }
        printf( "%c", c[i]);
        i++;
    }
    printf( "\n");
}

_Bool isnewline(const char c) {
    return c == '\n';
}

_Bool validint(int n) {
    return n > -1 && n < 10;
}

_Bool isnum(char c) {
    int n = char2int(c);

    return validint(n);
}

_Bool posnull(size_t pos) {
    return pos == ABC2_PARSE_NULL;
}

_Bool isdelimiter(char c) {
    return c == ABC2_DELIMITER;
}

void resetpos(size_t* a) {
    *a = ABC2_PARSE_NULL;
}

int char2int(char c) {
    return c - '0';
}

int ctoi(size_t len, char c[len]) {
    int i,j = 0;

    for( i=0;i<len;i++ ) {
        j = j*10 + c[i] - '0';
    }

    return j;
}

void reseterr() {
    decodeerr = 0;
    decodeerrdata.pos = 0;
    decodeerrdata.n = 0;
}