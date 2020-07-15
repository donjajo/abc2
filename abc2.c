#include "abc2.h"

#define CMD_LEN 6
#define MAX_KEYS 10
#define MAX_VALUES 4

void resetoptind( int optind[static 1] ) {
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
    if ( argc < 2 ) {
        printf( "Provide path to file\n" );
        return EXIT_FAILURE;
    }
    
    if ( export_key(argv[1]) ) {
        printf( "File exported successfully!\n" );
        return 0;
    }

    perror( argv[1] );
    return EXIT_FAILURE;
}

int main() {
    cmd cmds[CMD_LEN] = {
        { .cmd="q", .desc="Exit interactive shell", .func=exit_shell },
        { .cmd="h", .desc="Show help menu", .func=show_help},
        { .cmd="v", .desc="View keymaps", .func=view_keymaps},
        { .cmd="r", .desc="Remap key (e.g. r 9 a,b,c)", .func=remap_key },
        { .cmd="d", .desc="Decode an encrypted data using the keymaps", .func=decode },
        { .cmd="e", .desc="Export current mapping to a file", .func=exportkey },
    };

    char keymaps[11][4] = {
        { ' ' },
        { ',', '.', '!' },
        { 'a', 'b', 'c' },
        { 'd', 'e', 'f' },
        { 'g', 'h', 'i' },
        { 'j', 'k', 'l' },
        { 'm', 'n', 'o'},
        { 'p', 'q', 'r', 's' },
        { 't', 'u', 'v' },
        { 'w', 'x', 'y', 'z' },
        { '&', '-', '_'},
    };
    
    
    
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