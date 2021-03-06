#ifndef ABC2_ENCODER_H
	#define ABC2_ENCODER_H 1

	#include "keymap.h"
	#include "string.h"
	#include <time.h>

	#ifndef ABC2_EN_FORMAT
		#define ABC2_EN_FORMAT  L"%d %ld"
	#endif

	#ifndef ABC2_EN_DELIM
		#define ABC2_EN_DELIM   L"\t"
	#endif

	#ifndef ABC2_EN_LINEMAX
		#define ABC2_EN_LINEMAX  1024
	#endif

	#ifndef ABC2_EN_LINEINC
		#define ABC2_EN_LINEINC  64
	#endif

	#ifndef ABC2_EN_DEFAULT_RANDMAX
		#define ABC2_EN_DEFAULT_RANDMAX 5
	#endif

	#ifndef ABC2_EN_BUFMAX
		#define  ABC2_EN_BUFMAX 2
	#endif

	#ifndef ABC2_EN_BUFINCR
		#define ABC2_EN_BUFINCR 512
	#endif

	_Bool getencode(kv_int value, _Bool ischar, size_t n, wchar_t writebuf[n]);
	wchar_t* encode(char const* string, _Bool create_keymaps);
	int generate_rand(int max, int seed);
	size_t encode_string(char const* str, wchar_t** retbuf, _Bool create_keymaps);
	size_t encode_file(FILE* file, wchar_t** retbuf, _Bool create_keymaps);
#endif