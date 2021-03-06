#include "headers/decoder.h"

abc2parsed* _parse(wchar_t* pair) {
	abc2parsed* parsed = malloc(sizeof(abc2parsed));
	if (!parsed) {
		error_terminate(__func__, "malloc");
	}
	
	if (swscanf(pair, ABC2_EN_FORMAT, &(parsed->n), &(parsed->pos)) < 2) {
		free(parsed);
		return 0;
	}

	return parsed;
}

size_t decode(wchar_t* v, wchar_t** ret) {
	size_t n = 0;
	_Bool manage_mem = (!*ret);

	if (1) {
		abc2parsed* parsed;
		wchar_t c;
		size_t memalloced = sizeof(wchar_t[ABC2_DEC_BUFMAX]);
		wchar_t* saveptr;

		if (manage_mem) {
			*ret = malloc(memalloced);
			if (!*ret) {
				error_terminate(__func__, "malloc");
			}
		}

		for (wchar_t* pair = wcstok(v, ABC2_EN_DELIM, &saveptr); pair; pair = wcstok(0, ABC2_EN_DELIM, &saveptr), n++) {
			if (!(parsed = _parse(pair))) {
				printf("PARSE ERROR: \e[31m%ls\e[0m\n", pair);
				goto FREE_ERR;
			}

			c = _decode(parsed);
			if (!c) {
				printf("NO MAP: \e[31m%ls\e[0m\n", pair);
				free(parsed);
				goto FREE_ERR;
			}

			if (manage_mem)
				*ret = memincr(*ret, sizeof(wchar_t[n+1]), sizeof(wchar_t), &memalloced, sizeof(wchar_t[ABC2_DEC_BUFINC]));

			*((*ret)+n) = c;
			free(parsed);
		}

		if (manage_mem) {
			*ret = memrz(*ret, memalloced, sizeof(wchar_t[n+1]));
		}

		*((*ret)+(n)) = L'\0';

		return n+1;
	}
	
	return n;

	FREE_ERR:
	if (manage_mem && *ret) {
		free(*ret);
	} 
	return -1;
}

wchar_t _decode(abc2parsed* parsed) {
	if (parsed) {
		key* k = key_exists(parsed->n);
		if (!k || (parsed->pos >= k->len))
			return 0;

		if (is_char(parsed->pos, k->wcharcount, k->wchars)) {
			return (wchar_t) k->maps[parsed->pos];
		}  else {
			wchar_t b[2];
			swprintf(b, 2, L"%Ld", k->maps[parsed->pos]);
			return b[0];
		}
	}

	return 0;
}