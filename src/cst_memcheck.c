#define _GNU_SOURCE
#include "cst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <unistd.h>
#include <dlfcn.h>

#ifndef CST_MAX_BT
# define CST_MAX_BT 32
#endif

#ifndef CST_PATH_MAX
# define CST_PATH_MAX 1024
#endif

/*
 - Data structures
 */

/**
 * @brief Stores a backtrace.
 *
 * Holds up to CST_MAX_BT return addresses obtained from `backtrace()`.
 */
typedef struct cst_backtrace {
	// Number of captured frames
	int size;
	// Raw addresses
	void *addrs[CST_MAX_BT];
} cst_backtrace;

/**
 * @brief Node in the allocation tracking linked list.
 *
 * Each node represents one malloc'd block with its size and
 * the backtrace at allocation time.
 */
typedef struct cst_allocation {
	// Allocated pointer
	void *ptr;
	// Allocation size in bytes
	size_t size;
	// Backtrace at allocation
	cst_backtrace bt_alloc;
	// Next node
	struct cst_allocation *next;
} cst_allocation;

/** @brief Head of the allocation tracking list. */
static cst_allocation *alloc_list = NULL;

/* Real malloc/free */

void *__real_malloc(size_t size);
void __real_free(void *ptr);

/*
 - Backtrace utils
 */

/**
 * @brief Capture a backtrace of the current stack.
 *
 * @param bt cst_backtrace struct to fill.
 * @param skip Number of initial frames to skip (used to remove internal calls).
 */
static void bt_capture(cst_backtrace *bt, int skip) {
	int n = backtrace(bt->addrs, CST_MAX_BT);

	if (skip > 0 && skip < n) {
		memmove(bt->addrs, bt->addrs + skip, (n - skip) * sizeof(void *));
		n -= skip;
	} else if (skip >= n)
		n = 0;
	bt->size = n;
}

/**
 * @brief Run addr2line to resolve an address to source location.
 *
 * @param binary Path to the executable/library file.
 * @param addr   Address to resolve.
 * 
 * @return true if resolved successfully, false otherwise.
 */
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

/**
 * @brief Print symbol and source information for a single address.
 *
 * Uses dladdr() and addr2line as fallbacks.
 *
 * @param addr Address to resolve.
 * 
 * @return true if resolved successfully, false otherwise.
 */
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

/**
 * @brief Print a captured backtrace.
 *
 * Attempts to resolve each address to file/line information.
 * Falls back to backtrace_symbols() if resolution fails.
 *
 * @param bt  Captured backtrace.
 */
static void print_backtrace(const cst_backtrace *bt) {
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

/*
 - Allocation tracking
 */

/**
 * @brief Add a new allocation to the tracking list.
 *
 * @param ptr Pointer returned by malloc.
 * @param size Number of bytes allocated.
 * @param bt Backtrace of the allocation site.
 */
static void add_alloc(void *ptr, size_t size, const cst_backtrace *bt) {
	if (ptr == NULL)
		return;
	cst_allocation *node = (cst_allocation *)__real_malloc(sizeof(cst_allocation));
	node->ptr = ptr;
	node->size = size;
	node->bt_alloc = *bt;
	node->next = alloc_list;
	alloc_list = node;
}

/**
 * @brief Remove an allocation from the tracking list.
 *
 * @param ptr The pointer to free.
 * 
 * @return `true` if the allocation was tracked and removed, `false` otherwise.
 */
static bool remove_alloc_if_present(void *ptr) {
	cst_allocation *current = alloc_list;
	cst_allocation *prev = NULL;

	while (current != NULL) {
		if (current->ptr == ptr) {
			if (prev != NULL)
				prev->next = current->next;
			else
				alloc_list = current->next;
			__real_free(current);
			return true;
		}
		prev = current;
		current = current->next;
	}
	return false;
}

/**
 * @brief Report all memory leaks detected at program exit.
 *
 * Prints the size, address and backtrace of each unfreed allocation.
 * Terminates the program with `EXIT_FAILURE` if leaks are present.
 */
__attribute__((destructor))
static void report_leaks(void) {
	if (alloc_list == NULL)
		return;
	fprintf(stderr, "💧"CST_BRED" %s "CST_GRAY"-"CST_RED" Memory leaks detected"CST_GRAY":"CST_RES"\n", CST_TEST_NAME);
	for (cst_allocation *cur = alloc_list; cur != NULL; cur = cur->next) {
		fprintf(stderr, CST_GRAY"- "CST_BRED"%zu bytes "CST_RED"allocated at"CST_GRAY":"CST_RES"\n", cur->size);
		print_backtrace(&cur->bt_alloc);
	}
	exit(EXIT_FAILURE);
}

/*
 - Wrappers
 */

/**
 * @brief Malloc wrapper with allocation tracking.
 *
 * Captures a backtrace of the allocation site and stores it.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to allocated memory.
 */
void *__wrap_malloc(size_t size) {
	void *ptr = __real_malloc(size);
	cst_backtrace bt = {0};
	bt_capture(&bt, 2);
	add_alloc(ptr, size, &bt);
	return ptr;
}

/**
 * @brief Free wrapper with double-free detection.
 *
 * Removes the allocation from tracking if present.
 * If the pointer was not tracked, reports a double free and aborts.
 *
 * @param ptr Pointer to free.
 */
void __wrap_free(void *ptr) {
	if (ptr == NULL)
		return;
	if (!remove_alloc_if_present(ptr)) {
		fprintf(stderr, "💥"CST_BRED" %s "CST_GRAY"-"CST_RED" Double free detected"CST_GRAY":"CST_RES"\n", CST_TEST_NAME, ptr);
		cst_backtrace bt = {0};
		bt_capture(&bt, 1);
		print_backtrace(&bt);
		exit(EXIT_FAILURE);
	}
	__real_free(ptr);
}
