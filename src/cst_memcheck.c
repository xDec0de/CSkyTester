#define CST_NO_MEMCHECK  // Prevent recursive macro expansion
#include "cst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 - Allocation tracking structure
 */

typedef struct cst_alloc {
	void *ptr;
	size_t size;
	const char *file;
	int line;
	struct cst_alloc *next;
} cst_alloc;

static cst_alloc *g_allocs = NULL;
static bool g_memcheck_enabled = true;

/*
 - Helper: Add allocation to tracking list
 */

static void track_alloc(void *ptr, size_t size, const char *file, int line)
{
	if (!ptr || !g_memcheck_enabled)
		return;
	
	cst_alloc *node = malloc(sizeof(cst_alloc));
	if (!node) {
		fprintf(stderr, CST_BRED"CST: Failed to allocate tracking node\n"CST_RES);
		exit(EXIT_FAILURE);
	}
	
	node->ptr = ptr;
	node->size = size;
	node->file = file;
	node->line = line;
	node->next = g_allocs;
	g_allocs = node;
}

/*
 - Helper: Remove allocation from tracking list
 */

static bool untrack_alloc(void *ptr, const char *file, int line)
{
	if (!ptr)
		return true;
	
	if (!g_memcheck_enabled)
		return true;
	
	cst_alloc **curr = &g_allocs;
	while (*curr) {
		if ((*curr)->ptr == ptr) {
			cst_alloc *tmp = *curr;
			*curr = (*curr)->next;
			free(tmp);
			return true;
		}
		curr = &(*curr)->next;
	}
	
	// Not found = double free or freeing untracked memory
	fprintf(stderr, CST_BRED"💥 %s "CST_GRAY"-"CST_RED" Double free or invalid free at %s:%d"CST_RES"\n",
			CST_TEST_NAME, file, line);
	exit(EXIT_FAILURE);
	return false;
}

/*
 - Public API: Tracked allocations
 */

void* cst_malloc_impl(size_t size, const char *file, int line)
{
	void *ptr = malloc(size);
	if (ptr)
		track_alloc(ptr, size, file, line);
	return ptr;
}

void* cst_calloc_impl(size_t nmemb, size_t size, const char *file, int line)
{
	void *ptr = calloc(nmemb, size);
	if (ptr)
		track_alloc(ptr, nmemb * size, file, line);
	return ptr;
}

void* cst_realloc_impl(void *ptr, size_t size, const char *file, int line)
{
	if (ptr)
		untrack_alloc(ptr, file, line);
	
	void *new_ptr = realloc(ptr, size);
	if (new_ptr && size > 0)
		track_alloc(new_ptr, size, file, line);
	
	return new_ptr;
}

void cst_free_impl(void *ptr, const char *file, int line)
{
	if (!ptr)
		return;
	
	untrack_alloc(ptr, file, line);
	free(ptr);
}

/*
 - Public API: Manual leak checking
 */

bool cst_has_leaks(void)
{
	return g_allocs != NULL;
}

void cst_print_leaks(void)
{
	if (!g_allocs)
		return;  // Silent if no leaks
	
	size_t total_leaked = 0;
	size_t leak_count = 0;
	
	fprintf(stderr, CST_BRED"💧 %s "CST_GRAY"-"CST_RED" Memory leaks detected"CST_GRAY":"CST_RES"\n", CST_TEST_NAME);
	
	for (cst_alloc *a = g_allocs; a; a = a->next) {
		fprintf(stderr, CST_GRAY"  - "CST_BRED"%zu bytes "CST_RED"at %s:%d"CST_RES"\n",
				a->size, a->file, a->line);
		total_leaked += a->size;
		leak_count++;
	}
	
	fprintf(stderr, CST_BRED"  Total: %zu bytes in %zu allocation(s)"CST_RES"\n",
			total_leaked, leak_count);
}

void cst_reset_memcheck(void)
{
	while (g_allocs) {
		cst_alloc *tmp = g_allocs;
		g_allocs = g_allocs->next;
		free(tmp);
	}
}

/*
 - Check leaks before test exit (called explicitly)
 */

void cst_check_leaks_before_exit(void) {
	if (!g_memcheck_enabled)
		return;
	
	if (g_allocs != NULL) {
		cst_print_leaks();
		
		// Clean list to avoid double reports
		while (g_allocs) {
			cst_alloc *tmp = g_allocs;
			g_allocs = g_allocs->next;
			free(tmp);
		}
		
		exit(EXIT_FAILURE);  // Force test failure
	}
}

/*
 - Automatic leak report on abnormal exit (fallback)
 */

__attribute__((destructor))
static void cst_report_leaks(void)
{
	// This only runs if the process exits without calling cst_check_leaks_before_exit
	if (!g_memcheck_enabled || !g_allocs)
		return;
	
	cst_print_leaks();
}
