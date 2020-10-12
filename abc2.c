#include "abc2.h"

#define CMD_LEN 10
#define MAX_KEYS 10
#define MAX_VALUES 4

inline void resetoptind( int optind[static 1] ) {
    *optind = 1;
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

int decode( size_t argc, char** argv) {
    if ( argc <= 1 ) {
        printf( "Value expected\n" );
        return 1;
    }

    char* decoded = abc2_decode( argv[1] );
    if ( decoded ) {
        printf( "%s\n", decoded);
        free(decoded);
    } else if ( decodeerr == ABC2_ERR_NOKEY ) {
        printf( "ERR: No key mapping for the number: %d\n", decodeerrdata.n );
        return 1;
    } else if ( decodeerr == ABC2_ERR_NOMAP ) {
        printf( "ERR: No mapping for the position: %ld in %d\n", decodeerrdata.pos, decodeerrdata.n );
        return 1;
    }
    return 0;
}

int exportkey(size_t argc, char** argv) {
    if ( export( argc < 2 ? "" : argv[1] ) ) {
        return 0;
    }

    return EXIT_FAILURE;
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

int load_key(size_t argc, char* argv[argc]) {
    int opt;
    extern int optind;
    _Bool ignore_duplicates = 0;
    resetoptind(&optind);
   
    while( ( opt = getopt(argc, argv, "i" ) ) != -1 ) {
        switch( opt ) {
            case 'i':
                ignore_duplicates = 1;
                break;
            default:
                ignore_duplicates = 0;
                break;
        }
    }

    while( optind < argc ) {
        size_t len;
        struct keyfile* keys = load_keyfile(argv[optind], &len);
        
        if ( keys ) {
            for( size_t i = 0; i<len; i++ ) {
                struct keyfile key = keys[i];
                map_from_keyfile(key, ignore_duplicates);
            }
            free(keys);
        }
        optind++;
    }

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
                char ans[2];
                size_t len;
            
                while(1) {
                    printf( "%s already exist. Do you wish to overwrite? Y/N: ", dump);
                    fflush(stdout);
                    scanf( "%s", ans);
                    len = strlen(ans);
                    
                    if ( len == 1 && (ans[0] == 'Y' || ans[0] == 'N' ) ) {
                        break;
                    }
                }

                if ( ans[0] == 'N' ) {
                    fclose(file);
                    break;
                }
                
            }
            
            fprintf( stdout, "Encoding file contents..." );
            fflush(stdout);
            len = encode_file(file, &buf, create_keymaps);
            fprintf( stdout, "DONE\n");
            fflush(stdout);
            if ( len ) {
                fprintf(stdout, "Writing to %s...", dump);
                fflush(stdout);
                write_encode(dump, len, buf);
                fprintf( stdout, "DONE\n");
                fflush(stdout);
                free(buf);
            }
            fclose(file);
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
        { .cmd="decode", .desc="Decode an encrypted data using the keymaps", .func=decode },
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