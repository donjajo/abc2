#ifndef SHELL_H
	#define SHELL_H		1
	#define SHELL_HOOK_CUR	0
	#define SHELL_EXIT	-100
	#define CMDTMP_DESTORY	-1
	#define CMDTMP_GET	0
	#define CMDTMP_WRITE	1

	typedef int shell_exec();

	typedef struct {
		const char* cmd;
		const char* desc;
		int (*func)(size_t argc, char** argv);
		_Bool autorun;
	} cmd;

	int shell_init();
	int shell_run(size_t cmd_len_max);
	_Bool shell_hook(cmd cmd);
	cmd* shell_get_hook();
	size_t shell_hook_count(size_t c);
	void shell_help_menu();
	char* cmdtmpbuf(const char* c, int action);
	void shell_run_autoload();
#endif