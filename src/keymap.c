#include "keymap.h"

/**
 * View list of keymaps
 * 
 * @return  int     Exit status
 * 
*/
int view_keymaps() {
    size_t i,j;
    size_t c = key_count(KEY_LEN_CUR); // Get count of mapped objects
    key* obj = get_obj((void*)0); // Get the object pointer containing all mappings
    
    // Iterate and print all 
    for( i=0;i<c;i++) {
        printf( "%d: {", obj[i].n);
        for( j=0; j<obj[i].len;j++) {
            printf( "'%c'%c", obj[i].maps[j], (j+1!=obj[i].len ? ',' : '\0') );
        }
        printf( "}\n");
    }

    return 0;
}

/** 
 * Remap a key, appending a new value
 * 
 * @param   int     n       Number to map
 * @param   char*   values  Comma separated characters
 * 
 * @return  int     ret     Return status
 * 
*/
int remap_keymap(int n, char const* values ) {
    if ( !values )
        return REMAP_ERR_ISNULL;

    char const del[2] = ",";
    char* t = calloc(strlen(values)+1, sizeof(char)); // Helps us zero out the values
    strcpy(t, values);
    char* tokbuf = strtok( t, del);
    int ret = 0;

    for( ; tokbuf; tokbuf = strtok( NULL, del) ) {
        tokbuf = ltrim(tokbuf, ' ');
        tokbuf = rtrim(tokbuf, ' ', 0);
        size_t len = strlen(tokbuf);
        key* k;
        
        if ( len > 1 ) {
            ret = REMAP_ERR_CHAROVERFLOW;
            break; // I know, I would have just returned here. But I need to free(t) and I want to write it once
        }

        if ( len == 0 ) { // Because we stripped out spaces. It is mostly likey to be 0.  So, a space character was provided
            tokbuf[0]=' ';
        }

        if ( ( k = map_exists(tokbuf[0]) ) != NULL && k->n != n ) {
            ret = REMAP_ERR_MAP_EXISTS;
            break;
        }

        if ( k == NULL || (k != NULL && k->n != n) )
            map_one(n, tokbuf[0]);
    }

    free(t);    

    return ret;
}

/**
 * Has only one job. Increment or decrement a static count
 * 
 * @param   size_t     c   Number to sum
 * 
 * @return  size_t     Returns new sum
 * 
*/
size_t key_count(size_t c) {
    static size_t i=0;

    i += c;
    return i;
}

key* get_obj(key* nobj) {
    static key* p;
    
    // Got new object? Probably after a realloc. Set to new object
    if ( nobj ) {
        p = nobj;
        return NULL;
    }

    if ( !p ) {
        p = malloc(sizeof(key));
        if ( !p ) {
            perror("get_obj():malloc:");
            exit(EXIT_FAILURE);
        }
    }

    return p;
}

void destroy_obj() {
    key* obj;

    if ( (obj=get_obj(NULL)) != NULL ) {
        free(obj);
    }
}

key* key_exists( int n ) {
    key* obj = get_obj((void*)0);
    if ( !obj ) {
        return NULL;
    }

    size_t i;
    size_t len = key_count(KEY_LEN_CUR);

    for( i=0; i<len; i++ ) {
        if (obj[i].n == n) {
            return &obj[i];
        }
    }

    return NULL;
}

_Bool map_one(int n, char c) {
    key* k = key_exists(n);
    char* t;

    if ( k ) {
        t = k->maps;

        k->maps = malloc(sizeof(char[k->len+1])); // realloc(k->maps, k->len+1);
        if (!k->maps) {
            perror( "malloc" );
            return 0;
        }

        memmove(k->maps, t, k->len);

        k->maps[k->len] = c;
        k->len++;
        free(t);

        return 1;
    } else {
        char* a = malloc(sizeof(char[1]));
        a[0] = c;
        return map_num(n, 1, a);
    }
}

_Bool map_num( int num, size_t len, char maps[len] ) {
    key* k;
    key* obj;
    size_t count = key_count(KEY_LEN_CUR);
    
    // Count == 0? Then get_obj() has malloc of sizeof(key) already. Go ahead and set the first key
    if ( count == 0 ) {
        obj = get_obj(NULL);
        obj[0] = (key) {.len=len, .n=num, .maps=maps };

        key_count(+1); // One key set
        return 1;
    }

    if ( (k = key_exists(num)) == NULL ) {
        // Resize to allocate more space. Tricky part, count = 1, adding new element, we want to resize to count+1 then set for index=count (index starts at 0)
        if ( resize_obj(count+1) ) {
            obj = get_obj((void*)0);
            obj[count] = (key) {.len=len, .n=num, .maps=maps };
            key_count(+1); // Key is set, increase count
        } 
        return 0;
    } else {
        k->maps = maps;
        k->len = len;
    }

    return 1;
}

int resize_obj(size_t nsize) {
    key* obj = get_obj((void*)0);
    size_t osize = key_count(KEY_LEN_CUR);

    if ( nsize > osize ) {
        obj = realloc(obj, sizeof( key[nsize] ) );
        if ( !obj ) {
            perror( "realloc:" );
            return 0;
        }
        
        // New object created, pass to replace
        get_obj(obj);
        return 1;
    }

    return -1;
}

key* map_exists( char c ) {
    key* obj = get_obj(NULL);
    size_t s = key_count(KEY_LEN_CUR);
    size_t i,j;

    for(i=0;i<s;i++) {
        for(j=0;j<obj[i].len;j++) {
            if (obj[i].maps[j]==c) {
                return &obj[i];
            }
        }
    }

    return NULL;
}

_Bool __iter_obj(iter_f func, int argc, ...) {
    size_t s = key_count(KEY_LEN_CUR);
    if ( !s ) 
        return 0;

    key* keys = get_obj(NULL);
    size_t i;
    va_list arg_list;
    va_start(arg_list, argc);

    for(i=0;i<s;i++) {
        func(keys+i, argc, arg_list);
    }

    va_end(arg_list);

    return 1;
}

int findunit( int num, int sum ) {

    int x = num/( num < 0 ? -10 : 10);
    if ( num < 0 ) {
        sum++;
    }

    if ( x < 1 ) {
        return sum;
    }

    sum++;
    return findunit(x, sum);
}

void _write_line( key* k, int argc, va_list arglist ) {
    if ( argc < 1 )
        return;
    
    int digitcount = findunit(k->n, 1);
    size_t len = (k->len + digitcount + 2); // This will produce a line like this "48:abceowpsi"
    char buf[len];
    static int fd = 0;
    int i;
    if ( fd == 0 )
        fd = va_arg(arglist, int);

    memset(buf, 0, len);
    sprintf(buf, "%d:", k->n);
    for( i = 0; i<k->len; i++ ) {
        buf[digitcount+(i+1)] = k->maps[i];
    }
    buf[len-1]='\n';

    write(fd, buf, len);
}

_Bool export_key(char const* file) {
    int fd;
    mode_t mode = S_IRUSR|S_IWUSR;

    if ( ( fd = open( file, O_CREAT | O_TRUNC | O_WRONLY, mode ) ) < 0 ) {
        return 0;
    }

    __iter_obj(_write_line, 1, fd);
    close(fd);

    return 1;
}