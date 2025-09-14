
# > ~ Define project, tests and test suite directory - USE CURDIR TO AVOID ISSUES!

CST_DIR = $(CURDIR)/../src
SRCS_DIR = $(CURDIR)/src
TEST_DIR = $(CURDIR)/test
CST_BIN = $(CST_DIR)/cst

# > ~ Source lists

PROJ_SRC := $(shell find $(SRCS_DIR) -type f -name '*.c')
TEST_SRC := $(shell find $(TEST_DIR) -type f -name '*.c' | sort)
EXTRA_CFLAGS = -I$(SRCS_DIR)

test: $(CST_BIN)
	@$(CST_BIN) "proj_srcs=$(PROJ_SRC)" "test_srcs=$(TEST_SRC)" "cflags=$(EXTRA_CFLAGS)"

# > ~ (Optional) Test suite clean task to remove temporary files

clean:
	@rm -rf $(TEST_DIR)/cst

.PHONY: test clean

# > ~ (Optional) Avoid "Entering / Leaving directory" messages

MAKEFLAGS += --no-print-directory
