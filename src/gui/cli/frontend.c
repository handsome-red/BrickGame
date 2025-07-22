#include "./frontend.h"

/**
 * @brief Initializes ncurses library with default game settings
 *
 * Configures terminal for game input/output:
 * - Enables raw character input without line buffering
 * - Disables character echoing
 * - Enables extended keyboard input (F-keys, arrows)
 * - Sets non-blocking input mode
 * - Enables scrolling
 * - Hides cursor
 * - Configures mouse event timing
 */
void initialize_ncurses() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, 1);
  nodelay(stdscr, TRUE);
  scrollok(stdscr, 1);
  curs_set(0);
  mouseinterval(1);
}

UserAction_t readInput() {
  UserAction_t action = NO_ACTION;
  int ch = getch();

  switch (ch) {
    case 's':
      action = Start;
      break;
    case 'p':
      action = Pause;
      break;
    case 'q':
      action = Terminate;
      break;
    case KEY_LEFT:
      action = Left;
      break;
    case KEY_RIGHT:
      action = Right;
      break;
    case KEY_UP:
      action = Up;
      break;
    case KEY_DOWN:
      action = Down;
      break;
    case ' ':
      action = Action;
      break;
  }
  return action;
}

void render(GameInfo_t CurrentState) {
  clear();
  napms(50);
  if (CurrentState.field != NULL) {
    if (CurrentState.pause == PREVIEW) {
      draw_common_banner("I LOVE AIGUL <3", true);
    } else

        if (CurrentState.pause == PAUSE_ON) {
      draw_common_banner("PAUSED", true);
    } else {
      render_game_field(CurrentState.field);
      render_game_status(CurrentState);
    }
    refresh();
  }
}

void draw_common_banner(const char* banner_text, bool color_shift) {
  (void)color_shift;
  static bool colors_initialized = false;
  if (!colors_initialized) {
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_BLACK);
    colors_initialized = true;
  }
  static int frame = 0;
  frame = (frame < 3) ? frame + 1 : 0;

  for (int row = 0; row < GAME_FIELD_HEIGHT; row++) {
    for (int col = 0; col < GAME_FIELD_WIDTH; col++) {
      mvaddch(row, col * 2, ' ' | COLOR_PAIR(4));
      mvaddch(row, col * 2 + 1, ' ' | COLOR_PAIR(4));
    }
  }

  int y = 8;
  int x = 7;

  attron(A_BOLD | COLOR_PAIR(1 + frame % 3));
  mvprintw(y, x, "%s", banner_text);
  attrset(A_NORMAL);
}

void render_game_field(int** filed) {
  for (int row = 0; row < GAME_FIELD_HEIGHT; row++) {
    for (int col = 0; col < GAME_FIELD_WIDTH; col++) {
      render_cell(row, col, filed[row][col]);
    }
  }
}

void render_cell(int row, int col, bool is_filled) {
  const chtype ch = is_filled ? (' ' | A_REVERSE) : ACS_CKBOARD;
  mvaddch(row, col * 2, ch);
  mvaddch(row, col * 2 + 1, ch);
}

void render_game_status(GameInfo_t CurrentState) {
  const int sidebar_x = GAME_FIELD_WIDTH * 2 * 1.3;
  const int start_y = GAME_FIELD_HEIGHT - 12;

  char buffer[4][32];
  snprintf(buffer[0], sizeof(buffer[0]), "RECORD: %d", CurrentState.high_score);
  snprintf(buffer[1], sizeof(buffer[1]), "LEVEL : %d", CurrentState.level);
  snprintf(buffer[2], sizeof(buffer[2]), "SCORE : %d", CurrentState.score);
  snprintf(buffer[3], sizeof(buffer[3]), "SPEED : %d", CurrentState.speed);

  for (int i = 0; i < 4; i++) {
    mvaddstr(start_y + (i * 2), sidebar_x, buffer[i]);
  }

  print_next_figure(CurrentState);
}

void print_next_figure(GameInfo_t CurrentState) {
  for (int row = 0; row < BLOCK_SIZE; row++) {
    for (int col = 0; col < BLOCK_SIZE; col++) {
      mvaddch(NEXT_FIGURE_Y + row, NEXT_FIGURE_X + col * 2, ' ');
      mvaddch(NEXT_FIGURE_Y + row, NEXT_FIGURE_X + col * 2 + 1, ' ');
    }
  }

  mvprintw(NEXT_FIGURE_Y - 2, NEXT_FIGURE_X, "NEXT");

  for (int row = 0; row < BLOCK_SIZE; row++) {
    for (int col = 0; col < BLOCK_SIZE; col++) {
      if (CurrentState.next[row][col]) {
        mvaddch(NEXT_FIGURE_Y + row, NEXT_FIGURE_X + col * 2, ' ' | A_REVERSE);
        mvaddch(NEXT_FIGURE_Y + row, NEXT_FIGURE_X + col * 2 + 1,
                ' ' | A_REVERSE);
      }
    }
  }
}