BIN = tetris
FRONTEND_LIB = libtetris_frontend.a
BACKEND_LIB = libtetris_backend.a

CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lcheck

FRONTEND_SRC = gui/cli/frontend.c
BACKEND_SRC = brick_game/tetris/backend.c
TEST_SRC = test/backend_test.c
MAIN = main.c

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

play: install
	@echo "The game is starting"
	@./$(BIN)

test: clean $(BACKEND_LIB) $(TEST_OBJ)
		@$(CC) $(CFLAGS) $(TEST_OBJ) $(BACKEND_LIB) -o test/tests $(LDFLAGS)
		@$(TEST_DIR)/tests

uninstall: clean

clean:
	@echo "Cleaning up files"
	@rm -f $(CLEAN_FILES) $(BIN)

.PHONY: all clean install play