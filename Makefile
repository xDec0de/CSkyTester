# ============================================
# CSkyTester (CST) - GNU C99 Makefile
# ============================================

# === Configuration ===
DIR ?= /usr/local
LIBDIR = $(DIR)/lib
INCDIR = $(DIR)/include
BINDIR = $(DIR)/bin

SRC = src/cst.c
OBJ = cst.o
STATIC = libcst.a
SHARED = libcst.so

CC = gcc
CFLAGS = -std=gnu99 -O2 -fPIC -Wall -Wextra -Werror

# === Targets ===

all: $(STATIC) $(SHARED)
	@echo "✅ Build complete"

$(OBJ): $(SRC)
	@echo "🔧 Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(STATIC): $(OBJ)
	@echo "📦 Creating static library..."
	@ar rcs $@ $^

$(SHARED): $(OBJ)
	@echo "📦 Creating shared library..."
	@$(CC) -shared -o $@ $^

debug: CFLAGS = -std=gnu99 -g3 -O0 -fPIC -Wall -Wextra -Werror
debug: clean all
	@echo "🐞 Debug build complete (includes full macro info)"

release: clean all
	@echo "🚀 Release build complete"

install: all
	@echo "📂 Installing to $(DIR)..."
	@install -d $(LIBDIR) $(INCDIR)
	@install -m 0644 src/cst.h $(INCDIR)/
	@install -m 0644 $(STATIC) $(LIBDIR)/
	@install -m 0755 $(SHARED) $(LIBDIR)/
	@echo "🔄 Updating dynamic linker cache..."
	@sudo ldconfig $(LIBDIR) >/dev/null 2>&1 || echo "⚠️  Could not run ldconfig (non-root install)"
	@echo "✅ CST installed successfully at $(DIR)"

uninstall:
	@echo "🧹 Uninstalling CST from $(DIR)..."
	@rm -f $(LIBDIR)/$(STATIC) $(LIBDIR)/$(SHARED)
	@rm -f $(INCDIR)/cst.h
	@echo "✅ Uninstalled CST"

clean:
	@echo "🧽 Cleaning build artifacts..."
	@rm -f $(OBJ) $(STATIC) $(SHARED)

.PHONY: all install uninstall clean debug release
