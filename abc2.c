#include "abc2.h"

#define CMD_LEN 9
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
    };
    welcome();
    
    shell_init();
    init_hooks(CMD_LEN, cmds);
    shell_run(ARG_MAX);

    return 0;
}