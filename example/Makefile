
CST_DIR   = $(CURDIR)/../src
SRCS_DIR  = $(CURDIR)/src
TEST_DIR  = $(CURDIR)/test

CST_BIN = $(CURDIR)/cst

PROJ_SRCS := $(shell find $(SRCS_DIR) -type f -name '*.c')
TEST_SRCS := $(TEST_DIR)/bool.c $(TEST_DIR)/char.c $(TEST_DIR)/null.c $(TEST_DIR)/crash.c
CST_SRCS  := $(CST_DIR)/cst.c

ALL_SRCS  := $(PROJ_SRCS) $(TEST_SRCS) $(CST_SRCS)

CC = gcc
CFLAGS = -g3 -I$(CST_DIR) -I$(SRCS_DIR)

all: $(CST_BIN)

$(CST_BIN): $(ALL_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(CST_BIN)
	-@$(CST_BIN) -d
	@rm -rf $(CST_BIN)

clean:
	@rm -f $(CST_BIN)

.PHONY: all test clean

MAKEFLAGS += --no-print-directory
