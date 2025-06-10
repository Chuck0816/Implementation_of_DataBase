###############################################################################
#  Makefile                                                                   #
###############################################################################

# ─────────────────────────────────────────────────────────────────────────────
#  Toolchain & flags
# ─────────────────────────────────────────────────────────────────────────────
CXX        := clang++                # works on both Apple & Home-brew toolchains
BASEFLAGS  := -std=c++17 -g -O0 -Wall -Wextra -Werror -pedantic \
              -fsanitize=address -fno-omit-frame-pointer
INCLUDES   := -Iincludes
EXTRAFLAGS := -Wno-error=unused-parameter

CXXFLAGS := $(BASEFLAGS) $(INCLUDES) $(EXTRAFLAGS)

#  Variant *without* AddressSanitiser (for Catch executable only)
NO_ASAN   := $(filter-out -fsanitize=address,$(CXXFLAGS))

# ─────────────────────────────────────────────────────────────────────────────
#  Project sources
# ─────────────────────────────────────────────────────────────────────────────
LIB_SRCS      := src/db.cc src/db_table.cc
DRIVER_SRC    := src/driver.cc
DATABASE_BIN  := database

LEGACY_TEST_SRC := tests/tests.cc
LEGACY_TEST_BIN := legacy_tests

CATCH_TEST_SRCS := tests/catch_main.cc tests/tests_catch2.cc
CATCH_TEST_BIN  := catch_tests

# ─────────────────────────────────────────────────────────────────────────────
#  Default build (main program)
# ─────────────────────────────────────────────────────────────────────────────
.PHONY: all
all: $(DATABASE_BIN)

$(DATABASE_BIN): $(LIB_SRCS) $(DRIVER_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ─────────────────────────────────────────────────────────────────────────────
#  Legacy tests (hand-written main)
# ─────────────────────────────────────────────────────────────────────────────
$(LEGACY_TEST_BIN): $(LEGACY_TEST_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: run_legacy
run_legacy: $(LEGACY_TEST_BIN)
	./$(LEGACY_TEST_BIN)

# ─────────────────────────────────────────────────────────────────────────────
#  Catch-2 tests  (compiled **without** AddressSanitiser)
# ─────────────────────────────────────────────────────────────────────────────
$(CATCH_TEST_BIN): $(CATCH_TEST_SRCS) $(LIB_SRCS)
	$(CXX) $(NO_ASAN) $^ -o $@

.PHONY: run_catch
run_catch: $(CATCH_TEST_BIN)
	./$(CATCH_TEST_BIN)

# ─────────────────────────────────────────────────────────────────────────────
#  Build + run both suites
# ─────────────────────────────────────────────────────────────────────────────
.PHONY: tests
tests: run_legacy run_catch

# ─────────────────────────────────────────────────────────────────────────────
#  Clean – remove binaries, object files **and CSV data files**
# ─────────────────────────────────────────────────────────────────────────────
.PHONY: clean
clean:
	# Executables
	rm -f $(DATABASE_BIN) $(LEGACY_TEST_BIN) $(CATCH_TEST_BIN)
	# Objects
	rm -f src/*.o tests/*.o
	# Any CSV files produced while running the program / tests
	# (the ‘rm -f’ keeps silent if no matching files exist)
	rm -f -- *.csv

# end of Makefile