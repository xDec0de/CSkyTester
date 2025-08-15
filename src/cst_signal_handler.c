#include "cst_assertions.h"
#include <signal.h>

char *cst_getsigname(int signum)
{
	if (signum == SIGABRT)
		return ("SIGABRT");
	if (signum == SIGFPE)
		return ("SIGFPE");
	if (signum == SIGILL)
		return ("SIGILL");
	if (signum == SIGINT)
		return ("SIGINT");
	if (signum == SIGSEGV)
		return ("SIGSEGV / Segmentation fault");
	if (signum == SIGTERM)
		return ("SIGTERM");
	if (signum == SIGBUS)
		return ("SIGBUS");
	if (signum == SIGQUIT)
		return ("SIGQUIT");
	if (signum == SIGHUP)
		return ("SIGHUP");
	return ("???");
}

void cst_sighandler(int signum)
{
	fprintf(stderr, CST_RED"💥 %s "CST_GRAY"-"CST_RED" Crashed with signal %i (%s)"CST_RES"\n",
		CST_TEST_NAME, signum, cst_getsigname(signum));
	exit(EXIT_FAILURE);
}

__attribute__((constructor))
static void cst_auto_init(void)
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
