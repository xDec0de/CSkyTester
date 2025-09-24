
# > ~ Define project, tests and test suite directory - USE CURDIR TO AVOID ISSUES!

CST_DIR = $(CURDIR)/../src
CST_BIN = $(CST_DIR)/cst

SRCS_DIR = $(CURDIR)/src
TEST_DIR = $(CURDIR)/test

# > ~ Source lists

# Find test & project .c files
PROJ_SRC := $(shell find $(SRCS_DIR) -type f -name '*.c')
TEST_SRC := $(shell find $(TEST_DIR) -type f -name '*.c')

# Compile them to generate .o files
PROJ_OBJS = $(PROJ_SRC:%.c=%.o)
TEST_OBJS = $(TEST_SRC:%.c=%.o)

# > ~ C & CST flags

# We include CST's dir to access cst_assertions.h easily
CFLAGS = -I$(CST_DIR) -I$(SRCS_DIR)
# Enable debug mode on CST
CST_FLAGS = -d

test: $(PROJ_OBJS) $(TEST_OBJS) $(CST_BIN)
	@valgrind -q $(CST_BIN) "proj_objs=$(PROJ_OBJS)" "test_objs=$(TEST_OBJS)" $(CST_FLAGS)

# > ~ (Optional) Test suite clean task to remove temporary files

clean:
	@rm -rf $(PROJ_OBJS)
	@rm -rf $(TEST_OBJS)
	@rm -rf $(CST_DIR)/objs

.PHONY: test clean

# > ~ (Optional) Avoid "Entering / Leaving directory" messages

MAKEFLAGS += --no-print-directory
