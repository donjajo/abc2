#include "abc2.h"

#define CMD_LEN 10
#define MAX_KEYS 10
#define MAX_VALUES 4

inline void resetoptind( int optind[static 1] ) {
    *optind = 0;
}

int remap_key( size_t argc, char* argv[argc]) {
    int opt;
    int num;
    _Bool numset = 0;
    extern char* optarg;
    extern int optind;
    char* mapvalues = (void*)0;

    while( ( opt = getopt(argc, argv, "n:v:h" ) ) != -1 ) {
        switch( opt ) {
            case 'n':
                num = atoi(optarg);
                numset = 1;
                break;
            case 'v':
                mapvalues = optarg;
                break;
            case 'h':
                goto USAGE;
                break;
            default:
                goto USAGE;
        }
    }

    if ( !numset ) {
        printf( "Missing param: -n\n");
        goto USAGE;
    } else if( !mapvalues ) {
        printf( "Missing param: -v\n" );
        goto USAGE;
    }

    int remap = remap_keymap( num, mapvalues);

    if ( remap == REMAP_ERR_CHAROVERFLOW ) {
        printf( "ERROR: Only accepts a character separated with comma. (e.g. a,b,c NOT ab,c)\n" );
    } else if( remap == REMAP_ERR_MAP_EXISTS ) {
        printf( "ERROR: A character you are about to map is already mapped somewhere else. View list with v\n");
    } else {
        printf( "Keys map successfully. Run v to view list\n" );
    }
    
    resetoptind(&optind);
    return remap;

    USAGE:
        printf( "Usage: %s [-n number] [-v values]\n", argv[0] );
        resetoptind(&optind);
        return 1;
}

int decode_file( size_t argc, char** argv) {
    char *keyfile = 0, *output = 0;
    int n;
    extern int optind;
    FILE *outputf, *file;
    resetoptind(&optind);

    while( ( n = getopt(argc, argv, "k:o:h") ) != -1 ) {
        switch(n) {
            case 'k':
                keyfile = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 'h':
                goto HELP;
            default:
                keyfile = 0;
                output = 0;
                break;
        }
    }

    // Confirm the output file is accessible and writeable
    if ( output ) {
        if ( access(output, F_OK ) == 0 ) {
            char b[] = "Overwrite %s? Y/N: ";
            char confirm_msg[(sizeof(b)/sizeof(b[0]))+strlen(output)+1];
            sprintf(confirm_msg, b, output);

            if ( !confirm(confirm_msg, "Y", "N") ) 
                return 0;
        }

        outputf = fopen(output, "w");
        if ( !outputf ) {
            perror( output );
            return 1;
        }
    } else {
        outputf = stdout;
    }

    // Process key file
    if ( keyfile ) {
        keyfile_load(keyfile, 1);
    }

    if ( optind == argc ) {
        goto HELP;
    }

    file = fopen(argv[optind], "r");
    wchar_t buf[BUFSIZ+1];
    if ( !file ) {
        perror( argv[optind]);
        return 1;
    }

    for ( size_t n = fread(buf, sizeof(wchar_t), BUFSIZ, file);  n ; n = fread(buf, sizeof(wchar_t), BUFSIZ, file ) ) {
        buf[n] = L'\0';
        wchar_t* ret = 0;

        if ( decode(buf, &ret) != (size_t)-1) {
            fprintf( outputf, "%ls", ret);
            fflush(outputf);
            free(ret);
        }
    }

    if ( output )
        fclose(outputf);

    fclose(file);
    return 0;

    HELP:
    printf( "Decode a file\n\tdecode [..options] [FILE]\nOPTIONS:\n" );
    printf( "%5s\t%-5sOutput file of decoded text\n", "-o", "-" );
    printf( "%5s\t%-5sPath to key file\n", "-k", "-" );
    printf( "%5s\t%-5sShow help\n", "-h", "-" );
    return 2;
}

int exportkey(size_t argc, char** argv) {
    if ( argc < 2 ) {
        goto HELP;
    }

    if ( access( argv[1], F_OK ) == 0 ) {
        char msg[] = "Overwrite %s? Y/N: ";
        char msgf[sizeof(msg)/sizeof(msg[0]) + strlen(argv[1])];
        sprintf(msgf, msg, argv[1]);

        if ( !confirm(msgf, "Y", "N") ) {
            return 2;
        }
    }

    if ( export(argv[1]) ) {
        printf( "Key successfully dumped\n" );
        return 0;
    }
    
    return EXIT_FAILURE;

    HELP:
    printf( "Dump keyfile\n\tdump [FILE]\n" );
    return 1;
}

int unmap_key( size_t argc, char* argv[argc] ) {
    if ( argc < 2 ) {
        printf( "Usage: unmap [...maps]\n");
        return 1;
    }

    delete_keymaps(argv[1]);

    return 0;
}

void welcome() {
    printf( "===== ABC2 Encoder/Decoder ======\n" );
    printf( "Current Locale: %s\n\n", setlocale( LC_ALL, 0));
}

_Bool keyfile_load(char const* filename, _Bool ignore_dup) {
    size_t len;
    struct keyfile* keys = load_keyfile(filename, &len);

    if ( keys ) {
        for( size_t i = 0; i<len; i++ ) {
            struct keyfile key = keys[i];
            map_from_keyfile(key, ignore_dup);
        }
        free(keys);

        return 1;
    }
    return 0;
}

int load_key(size_t argc, char* argv[argc]) {
    int opt;
    extern int optind;
    _Bool ignore_duplicates = 0;
    resetoptind(&optind);
   
    while( ( opt = getopt(argc, argv, "ih" ) ) != -1 ) {
        switch( opt ) {
            case 'i':
                ignore_duplicates = 1;
                break;
            case 'h':
                goto HELP;
                break;
            default:
                ignore_duplicates = 0;
                break;
        }
    }

    if ( optind == argc ) {
        goto HELP;
    }

    while( optind < argc ) {
        keyfile_load(argv[optind], ignore_duplicates);
        optind++;
    }

    return 0;

    HELP:
    printf( "Load key file\nload [..options] [..FILE]\nOPTIONS:\n" );
    printf( "%5s\t%-5sIgnore duplicates. They are skipped without warning\n", "-i", "-" );
    printf( "%5s\t%-5sShow help menu\n", "-h", "-" );

    return 1;
}

int quit() {
    destroy_obj();
    return SHELL_EXIT;
}

inline char* createcopy( char const* src ) {
    size_t s = strlen(src)+1;
    char* b = malloc(sizeof(char[s]));
    if ( !b ) {
        error_terminate( __func__, "malloc" );
    }
    strcpy(b, src);
    return b;
}

void showencodehelp() {
    printf( "Encode a string or file\n\tencode [..options] [FILE]\nOPTIONS:\n" );
    printf( "%5s\t%-5sCreate keymaps if they do not exist\n", "-c", "-" );
    printf( "%5s\t%-5sShow help menu\n", "-h", "-" );
    printf( "%5s\t%-5sOutput encoded file name\n", "-o", "-" );
    printf( "%5s\t%-5sGenerate and dump keymap file to path\n", "-d", "-" );
    printf( "%5s\t%-5sEncode string from stdin\n", "-s", "-" );
}

int write_encode(void* restrict dest, size_t n, wchar_t encoded[n]) {
    FILE* f;
    if ( !dest ) {
        printf( "%ls\n", encoded); 
        return 0;
    } 
    
    f = fopen((char*) dest, "w");

    if ( !f ) {
        perror( "" );
        return EXIT_FAILURE;
    }

    fwrite(encoded, sizeof(wchar_t), n, f);
    fclose(f);
    
    return 0;
}

int makeencode(size_t argc, char** argv) {
    int opt;
    extern int optind;
    _Bool create_keymaps = 0;
    char *dumppath=0, *output=0, *toencode = 0;
    extern char* optarg;

    resetoptind(&optind);

    if ( argc < 2 ) {
        showencodehelp();
        return 1;
    }

    while( ( opt = getopt(argc, argv, "cho:d:s:M:" ) ) != -1 ) {
        switch( opt ) {
            case 'c':
                create_keymaps = 1;
                break;
            case 'o':
                output = createcopy(optarg);
                break;
            case 'd':
                dumppath = createcopy(optarg);
                break;
            case 's':
                toencode = createcopy(optarg);
                break;
            case 'h':
                showencodehelp();
                goto FREE;
                break;
            default:
                showencodehelp();
                goto FREE;
        }
    }

    if ( !toencode && optind == argc ) {
        printf( "encode ... Positional argument [FILE] is missing. Or pass a string to -s option\n" );
        goto FREE;
    }
    
    if ( toencode  ) {
        wchar_t* buf = 0;
        size_t len = encode_string(toencode, &buf, create_keymaps);
        write_encode(output, len, buf);
        free(buf);

        if ( dumppath ) {
            export(dumppath);
        }
        goto FREE;
    }

    if ( optind != argc ) {
        FILE* file;
        wchar_t* buf = 0;
        size_t len;
        char* dump_dir, *bname, *strbuf;

        for ( ; optind < argc; optind++ ) {
            
            file = fopen(argv[optind], "r");
            if ( !file ) {
                perror(argv[optind]);
                goto FREE;
            }
            
            dump_dir = getpath(argv[optind]);
            bname = basename(argv[optind]);
            
            if ( ( strbuf = strrchr_r(bname, '.') ) ) {
                bname = strbuf;
            }
            
            size_t l = strlen(dump_dir)+1+strlen(bname)+1+strlen(ABC2_DEFAULT_EXT);
            char dump[l];
            strcpy(dump, dump_dir);
            strcat(dump, "/");
            strcat(dump, bname);
            strcat(dump, ABC2_DEFAULT_EXT);
            dump[l-1] = '\0';
            free(dump_dir);
            if (strbuf )
                free(strbuf);
            
            if ( access(dump, F_OK) == 0 ) {
                char question[] = "%s already exist. Do you wish to overwrite? Y/N: ";
                char questionf[sizeof(question)/sizeof(question[0])+strlen(dump)];
                sprintf(questionf, question, dump);
                
                if ( !confirm(questionf, "Y", "N" ) ) {
                    fclose(file);
                    break;
                }
            }
            
            fprintf( stdout, "Encoding file contents..." );
            fflush(stdout);
            len = encode_file(file, &buf, create_keymaps);
            
            if ( len ) {
                fprintf( stdout, "DONE\n");
                fflush(stdout);
                fprintf(stdout, "Writing to %s...", dump);
                fflush(stdout);
                write_encode(dump, len, buf);
                fprintf( stdout, "DONE\n");
                fflush(stdout);
                free(buf);
            }
            fclose(file);
        }

        if ( dumppath ) {
            export(dumppath);
        }
    }
    
    goto FREE;

    return 0;

    FREE:
        if ( output ) 
            free(output);
        if (dumppath) 
            free(dumppath);
        if (toencode)
            free(toencode);
        return 0;
}

int main() {
    setlocale( LC_ALL, "" );
    
    cmd cmds[CMD_LEN] = {
        { .cmd="quit", .desc="Exit interactive shell", .func=quit },
        { .cmd="help", .desc="Show help menu", .func=show_help, .autorun=1 },
        { .cmd="list", .desc="View keymaps", .func=view_keymaps},
        { .cmd="map", .desc="Map keys (e.g. map -n 9 -v a,b,c)", .func=remap_key },
        { .cmd="decode", .desc="Decode an encrypted data using the keymaps", .func=decode_file },
        { .cmd="export", .desc="Export current keymaps to a key file", .func=exportkey },
        { .cmd="setlocale", .desc="Set Locale", .func=exportkey },
        { .cmd="unmap", .desc="Unmap a mapping", .func=unmap_key },
        { .cmd="load", .desc="Load key file", .func=load_key },
        { .cmd="encode", .desc="Encode a string/file", .func=makeencode },
    };
    welcome();
    
    shell_init();
    init_hooks(CMD_LEN, cmds);
    shell_run(ARG_MAX);

    return 0;
}