
# > ~ Avoid "Entering / Leaving directory" messages (Optional)

MAKEFLAGS += --no-print-directory

# > ~ Define CSkyTester directory

TESTER_DIR = ../src

# > ~ Define project directory, USE CUDIR TO AVOID ISSUES.

SRCS_DIR = $(CURDIR)/src

# > ~ Define tests directory, USE CUDIR TO AVOID ISSUES.

TEST_DIR = $(CURDIR)/test

# > ~ Call CSkyTester's Makefile

test:
	@$(MAKE) -C $(TESTER_DIR) PROJ_DIR="$(SRCS_DIR)" TESTS_DIR="$(TEST_DIR)"

# > ~ (Optional) Call CSkyTester's clean task

clean:
	@$(MAKE) clean -C $(TESTER_DIR) PROJ_DIR="$(SRCS_DIR)" TESTS_DIR="$(TEST_DIR)"

.PHONY: test clean
