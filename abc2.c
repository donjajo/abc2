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

int clear() {
    // for( size_t i = 0; i < 23; i++ ) {
    //     printf( "\n" );
    // }

    execl( "/usr/bin/clear", 0);
}

int main() {
    setlocale( LC_ALL, "" );
    
    cmd cmds[CMD_LEN] = {
        { .cmd="quit", .desc="Exit interactive shell", .func=exit_shell },
        { .cmd="help", .desc="Show help menu", .func=show_help, .autorun=1 },
        { .cmd="list", .desc="View keymaps", .func=view_keymaps},
        { .cmd="map", .desc="Map keys (e.g. map -n 9 -v a,b,c)", .func=remap_key },
        { .cmd="decode", .desc="Decode an encrypted data using the keymaps", .func=decode },
        { .cmd="export", .desc="Export current keymaps to a key file", .func=exportkey },
        { .cmd="setlocale", .desc="Set Locale", .func=exportkey },
        { .cmd="unmap", .desc="Unmap a mapping", .func=unmap_key },
        { .cmd="clear", .desc="Clear console", .func=clear },
    };
    welcome();
    // char keymaps[11][4] = {
    //     { ' ' },
    //     { ',', '.', '!' },
    //     { 'a', 'b', 'c' },
    //     { 'd', 'e', 'f' },
    //     { 'g', 'h', 'i' },
    //     { 'j', 'k', 'l' },
    //     { 'm', 'n', 'o'},
    //     { 'p', 'q', 'r', 's' },
    //     { 't', 'u', 'v' },
    //     { 'w', 'x', 'y', 'z' },
    //     { '&', '-', '_'},
    // };
    
    // map_num(0, 1, keymaps[0]);
    // map_num(1, 3, keymaps[1]);
    // map_num(2, 3, keymaps[2]);
    // map_num(3, 3, keymaps[3]);
    // map_num(4, 3, keymaps[4]);
    // map_num(5, 3, keymaps[5]);
    // map_num(6, 3, keymaps[6]);
    // map_num(7, 4, keymaps[7]);
    // map_num(8, 3, keymaps[8]);
    // map_num(9, 4, keymaps[9]);
    // map_num(-1, 3, keymaps[10]);
    
    shell_init();
    init_hooks(CMD_LEN, cmds);
    shell_run(ARG_MAX);
    
    destroy_obj();

    return 0;
}