#include "cst.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>

/*
 - From cst.c
 */

bool	cst_is_on_test(void);

/*
 - Signal handler
 */

static void cst_sighandler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM || signum == SIGQUIT || signum == SIGHUP) {
		if (!cst_is_on_test())
			fprintf(stderr, CST_BRED"❌ CST terminated by signal %i (%s)\n"CST_RES, signum, strsignal(signum));
	} else {
		fprintf(stderr, CST_BRED"💥 %s "CST_GRAY"-"CST_RED" Crashed with signal %i (%s)\n"CST_RES,
			CST_TEST_NAME, signum, strsignal(signum));
		cst_bt_print_current(2);
	}
	_exit(EXIT_FAILURE);
}

void cst_init_sighandler(void)
{
	static bool handling = false;

	if (handling)
		return;
	handling = true;
	// Crash signals
	signal(SIGABRT, cst_sighandler);
	signal(SIGFPE,  cst_sighandler);
	signal(SIGILL,  cst_sighandler);
	signal(SIGSEGV, cst_sighandler);
	signal(SIGBUS,  cst_sighandler);
	// Interruptions / terminations
	signal(SIGINT,  cst_sighandler);
	signal(SIGTERM, cst_sighandler);
	signal(SIGQUIT, cst_sighandler);
	signal(SIGHUP,  cst_sighandler);
}
