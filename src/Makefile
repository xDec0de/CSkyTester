
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
RES_R = $(RES)

CST_DIR = $(TESTS_DIR)/cst
LOG_DIR = $(CST_DIR)/logs
OBJ_DIR = $(CST_DIR)/objs
PROJ_OBJ_DIR = $(OBJ_DIR)/proj
TEST_OBJ_DIR = $(OBJ_DIR)/cst

# > ~ Sources

PROJ_SRC := $(shell find $(PROJ_DIR) -type f -name '*.c')
PROJ_SRC += $(CURDIR)/cst_config.c $(CURDIR)/cst_signal_handler.c $(CURDIR)/cst_memcheck.c
TEST_SRC := $(shell find $(TESTS_DIR) -maxdepth 1 -type f -name '*.c' | sort) \
            $(shell find $(TESTS_DIR) -mindepth 2 -type f -name '*.c' | sort)

# > ~ Objects

PROJ_OBJS = $(PROJ_SRC:$(PROJ_DIR)/%.c=$(PROJ_OBJ_DIR)/%.o)
TEST_OBJS = $(TEST_SRC:$(TESTS_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)

# > ~ Compilation

CFLAGS = $(EXTRA_CFLAGS) -I$(CURDIR) -g3 -fdiagnostics-color=always \
	-Wl,--wrap=malloc -Wl,--wrap=free

define compile_obj
	@mkdir -p $(dir $1)
	@echo -n "\r⏳ $(YLW)Compiling $(BYLW)$(notdir $2)$(GRAY)...$(RES_R)"
	@{ \
		ERR=$$($(CC) $(CFLAGS) -c $2 -o $1 2>&1); \
		if [ $$? -ne 0 ]; then \
			echo -n "\r❌ $(RED)Failed to compile $(BRED)$1$(GRAY):$(RES_R)"; \
			echo "\n$$ERR"; \
			exit 1; \
		fi; \
	}
	@echo -n "\r✅ $(GREEN)Successfully compiled $(notdir $2)$(RES_R)"
	@echo
endef

$(PROJ_OBJ_DIR)/%.o: $(PROJ_DIR)/%.c
	$(call compile_obj,$@,$<)

$(TEST_OBJ_DIR)/%.o: $(TESTS_DIR)/%.c
	$(call compile_obj,$@,$<)

# > ~ Main test rule

all: $(PROJ_OBJS) $(TEST_OBJS)
	@mkdir -p $(LOG_DIR)
	@FAILED=0; \
	TESTFILES="$(TEST_SRC)"; \
	CURRENT_CAT=""; \
	for FILE in $$TESTFILES; do \
		BASENAME=$$(basename $$FILE .c); \
		DIRNAME=$$(dirname $$FILE); \
		if [ -f "$$DIRNAME/cname" ]; then \
			CATNAME=$$(cat "$$DIRNAME/cname"); \
		else \
			CATNAME=""; \
		fi; \
		TEST_OBJ="$(TEST_OBJ_DIR)/$${FILE#$(TESTS_DIR)/}"; \
		TEST_OBJ="$${TEST_OBJ%.c}.o"; \
		EXEC=$$(mktemp); \
		COMPILE_OUTPUT=$$($(CC) $(CFLAGS) $(PROJ_OBJS) $$TEST_OBJ -o $$EXEC 2>&1); \
		if [ $$? -ne 0 ]; then \
			printf "❌ $(RED)Failed to compile test %s$(RES)\n" "$$BASENAME"; \
			echo "$$COMPILE_OUTPUT"; \
			FAILED=$$((FAILED + 1)); \
			rm -f $$EXEC; \
			continue; \
		fi; \
		if [ "$$CATNAME" != "" ] && [ "$$CURRENT_CAT" != "$$CATNAME" ]; then \
			printf "\n📁 $(BBLUE)%s$(GRAY):$(RES)\n" "$$CATNAME"; \
			CURRENT_CAT="$$CATNAME"; \
		fi; \
		OUTPUT=$$($$EXEC 2>&1); \
		STATUS=$$?; \
		rm -f $$EXEC; \
		if [ "$$CATNAME" != "" ]; then \
			if [ "$$FILE" = "$$(ls $$DIRNAME/*.c | tail -n1)" ]; then \
				PREFIX="└──"; \
				IS_LAST="yes"; \
			else \
				PREFIX="├──"; \
				IS_LAST="no"; \
			fi; \
			echo "$$OUTPUT" | awk -v first_prefix="$$PREFIX" -v is_last="$$IS_LAST" \
			'NR==1 {print first_prefix " " $$0} \
			 NR>1 {if (is_last=="yes") print "    " $$0; else print "│   " $$0}'; \
		else \
			echo "$$OUTPUT"; \
		fi; \
		if [ $$STATUS -ne 0 ]; then \
			FAILED=$$((FAILED + 1)); \
		fi; \
	done; \
	if [ $$FAILED -eq 0 ]; then \
		echo "\n✅ $(GREEN)All tests passed!$(RES)"; \
	else \
		echo "\n❌ $(BRED)$$FAILED test(s) failed$(RES)"; \
		exit 1; \
	fi

# > ~ Clean

clean:
	@if [ -d "$(CST_DIR)" ]; then \
		rm -rf "$(CST_DIR)"; \
		echo "\r✅ $(GREEN)Successfully cleaned test files$(RES)"; \
	else \
		echo "\r✅ $(GREEN)No test files left to clean. Just double checking I guess?$(RES)"; \
	fi

.PHONY: all clean
