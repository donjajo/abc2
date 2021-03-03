#ifndef ABC2_DECODER_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "keymap.h"
    #include "encoder.h"
    #include "types.h"

    #ifndef ABC2_DEC_BUFMAX
        #define ABC2_DEC_BUFMAX 5120
    #endif 

    #ifndef ABC2_DEC_BUFINC
        #define ABC2_DEC_BUFINC 2048
    #endif 

    size_t decode(wchar_t* v, wchar_t** ret);
    abc2parsed* _parse(wchar_t* pair);
    wchar_t _decode( abc2parsed* parsed );
#endif