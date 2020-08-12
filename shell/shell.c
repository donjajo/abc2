#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../src/headers/strings.h"
#include "args.h"
#include "shell.h"

cmd* CMDS;

int shell_init() {
    CMDS = malloc(sizeof(cmd));
    if (!CMDS) {
        return 0;
    }

    return 1;
}

char* cmdtmpbuf(const char* c, int action ) {
    static size_t len = 0;
    static char* buf;
    size_t c_len = 0;

    if ( action == CMDTMP_DESTORY && len ) {
        free(buf);
        len = 0;

        return NULL;
    } else if ( action == CMDTMP_GET ) {
        return buf;
    } else if ( c && action == CMDTMP_WRITE ) {
        c_len = strlen(c);

        if ( !len ) {
            buf = malloc(sizeof(char[c_len+1]));
            if ( !buf ) {
                printf( "%s:", __func__ );
                perror( "malloc:" );
                exit( EXIT_FAILURE );
            }
            strcpy(buf, c);
        } else {
            buf = realloc(buf, len+(c_len+1));
            if ( !buf ) {
                printf( "%s:", __func__ );
                perror( "realloc:" );
                exit( EXIT_FAILURE );
            }

            strcat(buf, c);
        }
        len += c_len+1;
    }

    return NULL;
}

int shell_run(size_t cmd_len_max) {
    char* c = malloc(sizeof( char[cmd_len_max+1] ) );
    wchar_t* default_prompt = L"\u27A4 ";
    wchar_t* prompt = default_prompt;
    char* tmp_buf;
    _Bool in_quotes = 0;

    size_t len;
    if (!c) {
        printf( "%s:", __func__ );
        perror( "malloc:" );
        exit(EXIT_FAILURE);
    }

    memset(c, 0, cmd_len_max+1);
    shell_run_autoload();
    while(1) {
        printf( "%ls", prompt );
        fgets(c, cmd_len_max+1, stdin);
        c = ltrim(ltrim(c, '\n'), ' ');
        c = in_quotes ? c : rtrim( c, '\n', 0);
        c = rtrim( c , ' ', 0);

        cmdtmpbuf(c, CMDTMP_WRITE);
        tmp_buf = cmdtmpbuf(NULL, CMDTMP_GET);
        len = strnlen(tmp_buf, cmd_len_max+1);

        if ( len > cmd_len_max ) {
            printf( "cmd_len_max reached!" );
            break;
        }
       
        if ( len == 0 ) { 
            continue;
        } else {
            if (unclosedquote(tmp_buf)) {
                prompt=L"";
                in_quotes = 1;
                continue;
            } else {
                prompt = default_prompt;
                in_quotes = 0;
            }

            
            size_t argc;
            char** argv = shell_split_arg(tmp_buf, &argc);
            
            cmd* cm = shell_get_hook(argv[0]);
            if (!cm) {
                printf( "%s: Invalid command!\n", tmp_buf);
                cmdtmpbuf(NULL, CMDTMP_DESTORY);
                free(argv);
                continue;
            } else {
                int status = cm->func(argc, argv);
                free(argv);
                if (status == SHELL_EXIT) {
                    cmdtmpbuf(NULL, CMDTMP_DESTORY);
                    break;
                }
                cmdtmpbuf(NULL, CMDTMP_DESTORY);
            }
        }
    }

    free(c);
    return 1;
}

size_t shell_hook_count(size_t c) {
    static size_t count=0;

    count+=c;
    return count;
}

inline void shell_run_autoload() {
    size_t hook_count = shell_hook_count(SHELL_HOOK_CUR);

    for( size_t i = 0; i<hook_count; i++ ) {
        if ( CMDS[i].autorun ) {
            CMDS[i].func(0, 0);
        }
    }
}

void shell_help_menu() {
    size_t hook_count = shell_hook_count(SHELL_HOOK_CUR);
    for( size_t i=0;i<hook_count;i++) {
        printf( "\t%s%3c %-30s\n", CMDS[i].cmd, '-', CMDS[i].desc);
    }
}

_Bool shell_hook( cmd cm ) {
    size_t i = shell_hook_count(SHELL_HOOK_CUR);

    if( !CMDS ) {
        fprintf(stderr, "shell_init(): not initialized\n" );
        exit(1);
    }

    if (i==0) {
        CMDS[i] = cm;
        i = shell_hook_count(+1);
    } else {
        i = shell_hook_count(+1);
        cmd* tmp = realloc(CMDS, sizeof( cmd[i] ) );
        if (!tmp) {
            return 0;
        }

        CMDS = tmp;
        CMDS[i-1] = cm;
    }

    return 1;
}

cmd* shell_get_hook( const char* cm ) {
    size_t hook_count = shell_hook_count(SHELL_HOOK_CUR);
    
    if ( hook_count ) {
        size_t i;
        for(i=0;i<hook_count;i++) {
            if ( !strcmp(CMDS[i].cmd, cm) ) {
                return &CMDS[i];
            }
        }
    }

    return NULL;
}