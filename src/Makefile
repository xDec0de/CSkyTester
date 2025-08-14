
MAKEFLAGS += --no-print-directory

ifndef PROJ_DIR
	$(error PROJ_DIR not defined; Can't execute tests.)
endif

ifndef TESTS_DIR
	$(error TESTS_DIR not defined; Can't execute tests.)
endif

# > ~ Colors!

RED = \033[0;31m
BRED = \033[1;31m
GREEN = \033[0;32m
BGREEN = \033[1;32m
YLW = \033[0;33m
BYLW = \033[1;33m
BLUE = \033[0;36m
BBLUE = \033[1;36m
GRAY = \033[0;30m
RES = \033[0m                                                                   

CST_DIR = $(TESTS_DIR)/cst
LOG_DIR = $(CST_DIR)/logs
OBJ_DIR = $(CST_DIR)/objs

TEST_SRC := $(shell find $(TESTS_DIR) -type f -name '*.c')
PROJ_SRC := $(shell find $(PROJ_DIR) -type f -name '*.c')
OBJS = $(PROJ_SRC:$(PROJ_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS ?= -Wall -Werror -Wextra
CFLAGS += -I$(CURDIR)

$(OBJ_DIR)/%.o: $(PROJ_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo -n "\r⏳ $(YLW)Compiling $(BYLW)$(notdir $<)$(GRAY)...$(RES)"
	@{\
		ERR=$$( ($(CC) $(CFLAGS) -c $< -o $@) 2>&1 );\
		if [ $$? -ne 0 ]; then\
			echo -n "\r❌ $(RED)Failed to compile $(BRED)$@$(GRAY):$(RES)";\
			echo "\n$$ERR";\
			exit 1;\
		fi;\
	}
	@echo -n "\r✅ $(GREEN)Successfully compiled project files$(RES)"
	@echo 

all: $(OBJS)
	@mkdir -p $(LOG_DIR)
	@FAILED=0; \
	TESTFILES="$(TEST_SRC)"; \
	for FILE in $$TESTFILES; do \
		BASENAME=$$(basename $$FILE .c); \
		if echo $$BASENAME | grep -q "^test_"; then \
			EXEC="$(LOG_DIR)/$$BASENAME.bin"; \
			LOG="$(LOG_DIR)/$$BASENAME.valgrind"; \
			ERR="$(LOG_DIR)/$$BASENAME.stderr"; \
			COMPILE_OUTPUT=$$($(CC) $(CFLAGS) $$FILE $(OBJS) -o $$EXEC 2>&1); \
			if [ $$? -ne 0 ]; then \
				printf "❌ $(RED)Failed to compile test $(BRED)%s$(GRAY):$(RES)\n" "$$BASENAME"; \
				echo "$$COMPILE_OUTPUT"; \
				FAILED=$$((FAILED + 1)); \
				continue; \
			fi; \
			if echo "$$BASENAME" | grep -q "_vg$$"; then \
				valgrind --leak-check=full --error-exitcode=123 --log-file="$$LOG" $$EXEC 2> $$ERR; \
			else \
				$$EXEC 2> $$ERR; \
			fi; \
			STATUS=$$?; \
			if [ $$STATUS -eq 0 ]; then \
				printf "✅ $(GREEN)%s passed$(RES)\n" "$$BASENAME"; \
			elif [ $$STATUS -eq 123 ]; then \
				printf "⚠️  $(BYLW)%s passed but has memory leaks$(RES)\n" "$$BASENAME"; \
				FAILED=$$((FAILED + 1)); \
			elif [ $$STATUS -gt 0 ] && [ $$STATUS -lt 128 ]; then \
				printf "❌ $(RED)%s test failed$(GRAY):$(RED) " "$$BASENAME"; \
				cat $$ERR; \
				printf "$(RES)\n"; \
				FAILED=$$((FAILED + 1)); \
			else \
				SIGNAL=$$(( $$STATUS - 128 )); \
				printf "💥 $(BRED)%s crashed with signal %d$(RES)\n" "$$BASENAME" "$$SIGNAL"; \
				FAILED=$$((FAILED + 1)); \
			fi; \
			rm -f $$ERR; \
		fi; \
	done; \
	rm -rf $(LOG_DIR)/*.bin; \
	if [ $$FAILED -eq 0 ]; then \
		echo "$(GREEN)✅ All tests passed!$(RES)"; \
	else \
		echo "$(RED)❌ $$FAILED test(s) failed.$(RES)"; \
		exit 1; \
	fi

clean:
	@if [ -d "$(CST_DIR)" ]; then \
		rm -rf "$(CST_DIR)"; \
		echo "\r✅ $(GREEN)Successfully cleaned test files$(RES)"; \
	else \
		echo "\r✅ $(GREEN)No test files left to clean. Just double checking I guess?$(RES)"; \
	fi

.PHONY: all clean
