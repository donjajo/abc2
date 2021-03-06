#ifndef SHELL_ARGS_H
	#define SHELL_ARGS_H 1
	_Bool issinglequote(const char c);
	_Bool isdoublequote(const char c);
	char** shell_split_arg(char* cm, size_t* argc);
#endif