
# > ~ Define project, tests and test suite directory - USE CURDIR TO AVOID ISSUES!

CST_DIR = $(CURDIR)/../src
SRCS_DIR = $(CURDIR)/src
TEST_DIR = $(CURDIR)/test

# > ~ Call test suite with extra CFLAGS

EXTRA_CFLAGS = -Wall -Werror -Wextra -I$(CURDIR)/src

test:
	@$(MAKE) -C $(CST_DIR) PROJ_DIR="$(SRCS_DIR)" TESTS_DIR="$(TEST_DIR)" EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

# > ~ (Optional) Test suite clean task to remove temporary files

clean:
	@$(MAKE) clean -C $(CST_DIR) PROJ_DIR="$(SRCS_DIR)" TESTS_DIR="$(TEST_DIR)"

.PHONY: test clean

# > ~ (Optional) Avoid "Entering / Leaving directory" messages

MAKEFLAGS += --no-print-directory
