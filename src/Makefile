BIN = tetris
FRONTEND_LIB = libtetris_frontend.a
BACKEND_LIB = libtetris_backend.a

CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lcheck

LOGS_DIR = ./tests/logs
FRONTEND_SRC = gui/cli/frontend.c
BACKEND_SRC = brick_game/tetris/backend.c
TEST_SRC = test/backend_test.c
DIST_DIR = dist
MAIN = main.c

DIST_NAME = brick_game_tetris.tar.gz
DIST_FILES = $(FRONTEND_SRC) $(BACKEND_SRC) common Makefile Doxyfile *.c
FRONTEND_OBJ = $(FRONTEND_SRC:.c=.o)
BACKEND_OBJ = $(BACKEND_SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)
MAIN_OBJ = $(MAIN:.c=.o)


CLEAN_FILES = $(FRONTEND_OBJ) $(BACKEND_OBJ) $(MAIN_OBJ) $(FRONTEND_LIB) $(BACKEND_LIB)

all: uninstall install play

$(FRONTEND_LIB): $(FRONTEND_OBJ)
	@ar rcs $@ $^

$(BACKEND_LIB): $(BACKEND_OBJ)
	@ar rcs $@ $^

install: $(MAIN_OBJ) $(BACKEND_LIB) $(FRONTEND_LIB)
	@$(CC) $(CFLAGS) -o $(BIN) $(MAIN_OBJ) $(BACKEND_OBJ) $(FRONTEND_OBJ) -lncurses
	@echo "Cleaning up library and object files..."
	@rm -f $(CLEAN_FILES)
	
uninstall: clean
	@rm -rf ./$(BIN)

play: install
	@echo "The game is starting"
	@./$(BIN)

test: clean $(BACKEND_LIB) $(TEST_OBJ)
		@$(CC) $(CFLAGS) $(TEST_OBJ) $(BACKEND_LIB) -o test/tests $(LDFLAGS)
		@./test/tests

gcov_report: CFLAGS += -fprofile-arcs -ftest-coverage
gcov_report: LDFLAGS += -lgcov -coverage
gcov_report: clean test
	@mkdir -p $(LOGS_DIR)
	@gcov -b brick_game/tetris/backend.c
	@lcov --capture --directory . --output-file $(LOGS_DIR)/coverage.info --rc lcov_branch_coverage=1
	@genhtml $(LOGS_DIR)/coverage.info --output-directory $(LOGS_DIR)/coverage_report --branch-coverage
	@open $(LOGS_DIR)/coverage_report/index.html

dvi:
	@rm -rf doxygen
	@doxygen && open doxygen/html/index.html

dist: clean
	@echo "Creating archive..."
	@mkdir -p $(DIST_DIR)
	@tar -czf $(DIST_DIR)/$(DIST_NAME) $(DIST_FILES)
	@echo "Archive created!"

valgrind: clean test
	@clear
	@valgrind --tool=memcheck --leak-check=yes test/tests

clang:
	@echo "Checking clang-format..."
	@cp ../materials/linters/.clang-format .
	@clang-format -n *.c *.h brick_game/tetris/*.c brick_game/tetris/*.h $(FRONT_DIR)/*.c ./headers/*.h ./tests/*.c ./tests/*.h
	@echo "Checking complete!"

clean:
	@echo "Cleaning up files"
	@rm -f $(CLEAN_FILES) $(BIN)
	@rm -rf ./test/tests ./test/backend_test.o ./test/backend_test.g* ./tests ./log.txt backend.c.gcov ./html ./brick_game/tetris/backend.g*

.PHONY: all clean install play