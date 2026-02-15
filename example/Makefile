CST_DIR   = $(CURDIR)/..
SRCS_DIR  = $(CURDIR)/src
TEST_DIR  = $(CURDIR)/test

CST_BIN = $(CURDIR)/cst
CST_LIB = $(CST_DIR)/libcst.a

CC = gcc
CFLAGS = -g3 -I$(SRCS_DIR) -I$(CST_DIR)/src

PROJ_SRCS := $(shell find $(SRCS_DIR) -type f -name '*.c' -exec basename {} \;)
TEST_SRCS := $(shell find $(TEST_DIR) -type f -name '*.c' -exec basename {} \;)

PROJ_SRCS := $(addprefix $(SRCS_DIR)/, $(PROJ_SRCS))
TEST_SRCS := $(addprefix $(TEST_DIR)/, $(TEST_SRCS))

OBJ_DIR = $(CURDIR)/objs
SRC_OBJS = $(PROJ_SRCS:$(SRCS_DIR)/%.c=$(OBJ_DIR)/src/%.o)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(OBJ_DIR)/test/%.o)

VALGRIND = 

all: test

$(OBJ_DIR)/src/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

test: $(SRC_OBJS) $(TEST_OBJS)
	@make -C $(CST_DIR)
	@$(CC) $(CFLAGS) $(SRC_OBJS) $(TEST_OBJS) $(CST_LIB) -o $(CST_BIN)
	-@$(VALGRIND) $(CST_BIN)
	@rm -rf $(CST_BIN)

valgrind:
	@$(MAKE) test VALGRIND="valgrind --leak-check=full --error-exitcode=1 --quiet"

clean:
	@rm -f $(CST_BIN)
	@rm -rf $(OBJ_DIR)

.PHONY: all test clean valgrind

MAKEFLAGS += --no-print-directory
