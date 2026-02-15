
CST_DIR   = $(CURDIR)/..
SRCS_DIR  = $(CURDIR)/src
TEST_DIR  = $(CURDIR)/test

CST_BIN = $(CURDIR)/cst
CST_LIB = $(CST_DIR)/libcst.a

CC = gcc
CFLAGS = -g3 -I$(SRCS_DIR) -I$(CST_DIR)/src

PROJ_SRCS := $(shell find $(SRCS_DIR) -type f -name '*.c')
TEST_SRCS := $(shell find $(TEST_DIR) -type f -name '*.c')

SRC_OBJS = $(PROJ_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_DIR)/%.o)

VALGRIND = 

all: test

test: $(SRC_OBJS) $(TEST_OBJS)
	@make -C $(CST_DIR)
	@$(CC) $(CFLAGS) $(SRC_OBJS) $(TEST_OBJS) $(CST_LIB) -o $(CST_BIN)
	-@$(VALGRIND) $(CST_BIN)
	@rm -rf $(CST_BIN)

valgrind:
	@$(MAKE) test VALGRIND="valgrind --leak-check=full --error-exitcode=1 --quiet"

clean:
	@rm -f $(CST_BIN)
	@rm -f $(SRC_OBJS)
	@rm -f $(TEST_OBJS)

.PHONY: all test clean

MAKEFLAGS += --no-print-directory
