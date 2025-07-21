# ============================================================================
# Generic C Project Makefile (flat /src, /build, /include layout)
# ============================================================================
#
# Project directory layout this Makefile expects (relative to its own location):
#
#   project_root/
#     Makefile            <-- this file
#     src/                <-- ALL compilable .c sources live *directly* in here
#     include/            <-- public headers (.h) used by sources; added to -I
#     build/              <-- generated object (.o) & dependency (.d) files
#
# The final executable is written to the *project root* (same dir as Makefile).
#
# -----------------------------------------------------------------------------
# QUICK START
# -----------------------------------------------------------------------------
# 1. Edit PROJECT below to the name you want for the output executable.
# 2. Add/adjust compiler flags (CFLAGS), preprocessor flags (CPPFLAGS),
#    linker search paths (LDFLAGS), and libraries (LDLIBS) as needed.
# 3. Run `make` to build.
# 4. Run `make run` to execute, or `make debug` to start gdb.
#
# -----------------------------------------------------------------------------
# USER CONFIGURATION SECTION
# -----------------------------------------------------------------------------

# Final executable name (no path)
PROJECT       := cdTest

# Directory names (relative paths). DO NOT end with a slash.
SRCDIR        := src
INCLUDEDIR    := include
BUILDDIR      := build

# Toolchain
CC            := gcc
# Choose your language standard
CSTD          := -std=c11    # change to -std=gnu11, -std=c99, etc.

# Warnings & optimizations
WARNINGS      := 
# -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wstrict-prototypes -Wconversion -Wsign-conversion
OPT           := -O0          # use -O0 for debugging, -Og for dev-friendly opt
DEBUGINFO     := -g3          # or -g0 to strip

# Preprocessor flags (e.g., -DDEBUG, include paths). We'll add -I$(INCLUDEDIR) below.
CPPFLAGS      :=

# Compiler flags (things that affect compilation proper):
CFLAGS        := $(CSTD) $(WARNINGS) $(OPT) $(DEBUGINFO)

# Linker flags: search paths (-L...) and linker control options (-Wl,...)
LDFLAGS       :=

# Libraries to link against (order matters; put dependent libs last!)
# e.g., -lm -lpng -lz -lncursesw
LDLIBS        := -lncurses

# -----------------------------------------------------------------------------
# INTERNAL/DERIVED SETTINGS (you usually don't need to change these)
# -----------------------------------------------------------------------------

# Always include our public headers path during compilation
CPPFLAGS      += -I$(INCLUDEDIR)

# Enable automatic dependency generation: produce .d files next to .o files.
CPPFLAGS      += -MMD -MP

# Collect ALL top-level .c files *directly* in SRCDIR (no recursion)
SRCS          := $(wildcard $(SRCDIR)/*.c)

# Map each source to an object in BUILDDIR (flat; filename only, no path mirroring)
# NOTE: If you have duplicate basenames in src/ (foo.c and sub/foo.c) this flat
# mapping would collide — but we are explicitly *not* recursing, so you're safe.
OBJS          := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

# Auto-generated dependency files (.d) corresponding to each object
DEPS          := $(OBJS:.o=.d)

# -----------------------------------------------------------------------------
# TOP-LEVEL TARGETS
# -----------------------------------------------------------------------------
.PHONY: all clean distclean rebuild run debug print-vars

all: $(PROJECT)

# Link step: produce final executable in project root
$(PROJECT): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Pattern rule: build object files in build/ from matching sources in src/
# The order-only prerequisite "| $(BUILDDIR)" ensures build/ exists.
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Ensure build directory exists (mkdir -p is idempotent)
$(BUILDDIR):
	@mkdir -p $@

# Convenience targets ---------------------------------------------------------
clean:
	rm -f $(OBJS) $(DEPS)

# distclean removes everything built, including the final executable
distclean: clean
	rm -f $(PROJECT)

# rebuild from scratch
rebuild: distclean all

# Run the program (rebuild if needed)
run: $(PROJECT)
	./$(PROJECT)

# Launch in gdb (requires gdb in PATH)
debug: $(PROJECT)
	gdb --args ./$(PROJECT)

# Print internal vars (debug Makefile configuration)
print-vars:
	@echo "PROJECT     = $(PROJECT)"
	@echo "SRCDIR      = $(SRCDIR)"
	@echo "INCLUDEDIR  = $(INCLUDEDIR)"
	@echo "BUILDDIR    = $(BUILDDIR)"
	@echo "SRCS        = $(SRCS)"
	@echo "OBJS        = $(OBJS)"
	@echo "CFLAGS      = $(CFLAGS)"
	@echo "CPPFLAGS    = $(CPPFLAGS)"
	@echo "LDFLAGS     = $(LDFLAGS)"
	@echo "LDLIBS      = $(LDLIBS)"

# -----------------------------------------------------------------------------
# AUTO-GENERATED DEPENDENCIES
# -----------------------------------------------------------------------------
# Each .o build (with -MMD -MP) creates a .d file listing the headers it depends on.
# Including them here lets Make rebuild things when headers change.
-include $(DEPS)

# -----------------------------------------------------------------------------
# NOTES / HOW-TO CUSTOMIZE
# -----------------------------------------------------------------------------
# 1. Add Libraries:
#      To link libpng + zlib + math:
#        LDLIBS := -lpng -lz -lm
#      To link ncursesw:
#        LDLIBS := -lncursesw
#   You can combine: LDLIBS := -lpng -lz -lm -lncursesw
#
# 2. Extra Library Search Paths:
#      LDFLAGS := -L/usr/local/lib -Wl,-rpath,/usr/local/lib
#
# 3. Compile-Time Defines:
#      CPPFLAGS += -DDEBUG=1 -DVERSION=\"1.2.3\"
#
# 4. Switch Between Debug & Release:
#      make OPT=-O0 DEBUGINFO=-g3   # debug build
#      make OPT=-O3 DEBUGINFO=-g0   # release build
#
# 5. Override PROJECT name on the command line:
#      make PROJECT=myprog
#
# 6. Verbose build (see all commands):
#      make V=1
#    By default Make prints the recipe line; you can wrap commands in @ if you
#    want silence. (This template shows most commands for clarity.)
#
# 7. Using pkg-config (example for libpng):
#      CPPFLAGS += $(shell pkg-config --cflags libpng)
#      LDLIBS   += $(shell pkg-config --libs   libpng)
#
# Enjoy!  — Generic Makefile tailored for /src, /build, /include
