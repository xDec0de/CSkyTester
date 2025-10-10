#define _GNU_SOURCE
#include "cst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <unistd.h>
#include <dlfcn.h>

void cst_bt_capture(cst_backtrace *bt, int skip) {
	if (bt == NULL)
		return;
	int n = backtrace(bt->addrs, CST_MAX_BT);
	if (skip > 0 && skip < n) {
		memmove(bt->addrs, bt->addrs + skip, (n - skip) * sizeof(void *));
		n -= skip;
	} else if (skip >= n)
		n = 0;
	bt->size = n;
}

static bool resolve_with_addr2line(const char *binary, unsigned long addr) {
	char cmd[CST_PATH_MAX];
	snprintf(cmd, sizeof(cmd), "addr2line -f -p -e '%s' %lx 2>/dev/null", binary, addr);

	FILE *fp = popen(cmd, "r");
	if (fp == NULL)
		return false;

	char line[CST_PATH_MAX];
	if (fgets(line, sizeof(line), fp) != NULL) {
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
		fprintf(stderr, "    "CST_RED"%s"CST_RES"\n", line);
		pclose(fp);
		return true;
	}
	pclose(fp);
	return false;
}

static bool print_addr_module(void *addr) {
	Dl_info info = {0};

	if (!dladdr(addr, &info) || info.dli_fname == NULL || info.dli_fbase == NULL) {
		char exep[CST_PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", exep, sizeof(exep) - 1);
		if (len > 0)
			exep[len] = '\0';
		else
			strcpy(exep, "/proc/self/exe");
		return resolve_with_addr2line(exep, (unsigned long) addr);
	}

	unsigned long base = (unsigned long)info.dli_fbase;
	unsigned long a_mod = (unsigned long)addr - base;

	if (resolve_with_addr2line(info.dli_fname, a_mod))
		return true;
	if (info.dli_sname != NULL) {
		fprintf(stderr, CST_RED"    at %s (%p)"CST_RES"\n", info.dli_sname, addr);
		return true;
	}
	return false;
}

void cst_bt_print(const cst_backtrace *bt) {
	if (bt == NULL || bt->size == 0) {
		fprintf(stderr, "  <no backtrace>\n");
		return;
	}
	bool resolved_any = false;
	for (int i = 0; i < bt->size; ++i)
		if (print_addr_module(bt->addrs[i]))
			resolved_any = true;
	if (!resolved_any) {
		char **syms = backtrace_symbols(bt->addrs, bt->size);
		if (syms != NULL) {
			for (int i = 0; i < bt->size; ++i)
				fprintf(stderr, "    %s\n", syms[i]);
			free(syms);
		}
	}
}

void cst_bt_print_current(int skip) {
	cst_backtrace bt = {0};
	cst_bt_capture(&bt, skip + 1);
	cst_bt_print(&bt);
}
