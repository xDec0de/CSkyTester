#include "cst_assertions.h"

typedef struct AllocNode {
	void *ptr;
	size_t size;
	struct AllocNode *next;
} AllocNode;

static AllocNode *alloc_list = NULL;

void *__real_malloc(size_t size);
void __real_free(void *ptr);

void add_alloc(void *ptr, size_t size) {
	if (!ptr)
		return;
	AllocNode *node = (AllocNode *)__real_malloc(sizeof(AllocNode));
	node->ptr = ptr;
	node->size = size;
	node->next = alloc_list;
	alloc_list = node;
}

void remove_alloc(void *ptr) {
	AllocNode **current = &alloc_list;
	while (*current) {
		if ((*current)->ptr == ptr) {
			AllocNode *tmp = *current;
			*current = tmp->next;
			__real_free(tmp);
			return;
		}
		current = &((*current)->next);
	}
	fprintf(stderr, "💥"CST_RED" %s "CST_GRAY"-"CST_RED" Double free detected at %p"CST_RES"\n", CST_TEST_NAME, ptr);
	exit(1);
}

void report_leaks(void) {
	AllocNode *current = alloc_list;
	if (!current)
		return;
	fprintf(stderr, "⚠️  Memory leaks detected:\n");
	while (current) {
		fprintf(stderr, "  - %zu bytes at %p\n", current->size, current->ptr);
		current = current->next;
	}
}

void *__wrap_malloc(size_t size) {
	void *ptr = __real_malloc(size);
	add_alloc(ptr, size);
	return ptr;
}

void __wrap_free(void *ptr) {
	if (!ptr)
		return;
	remove_alloc(ptr);
	__real_free(ptr);
}

__attribute__((destructor))
static void memcheck_destructor(void) {
	report_leaks();
}
