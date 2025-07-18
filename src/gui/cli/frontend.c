#include "./frontend.h"

void init_ncurses() {
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
  UserAction_t action = -1;
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

void print_game_field(GameInfo_t CurrentState) {
  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    for (int j = 0; j < GAME_FIELD_WIDTH; j++) {
      if (CurrentState.field[i][j] == 0) {
        mvaddch(i, j * 2, ACS_CKBOARD);
        mvaddch(i, j * 2 + 1, ACS_CKBOARD);
      } else {
        mvaddch(i, j * 2, ' ' | A_REVERSE);
        mvaddch(i, j * 2 + 1, ' ' | A_REVERSE);
      }
    }
  }
}

void print_game_info(GameInfo_t CurrentState) {
  char str_record[100] = {0};
  sprintf(str_record, "RECORD: %d", CurrentState.high_score);

  char str_level[100] = {0};
  sprintf(str_level, "LEVEL : %d", CurrentState.level);

  char str_score[100] = {0};
  sprintf(str_score, "SCORE : %d", CurrentState.score);

  char str_pause[100] = {0};

  sprintf(str_pause, "PAUSE : %s", CurrentState.pause ? "ON " : "OFF");

  print_next_figure(CurrentState);

  mvaddstr(GAME_FIELD_HEIGHT - 12, GAME_FIELD_WIDTH * 2 * 1.3, str_pause);
  mvaddstr(GAME_FIELD_HEIGHT - 10, GAME_FIELD_WIDTH * 2 * 1.3, str_score);
  mvaddstr(GAME_FIELD_HEIGHT - 8, GAME_FIELD_WIDTH * 2 * 1.3, str_level);
  mvaddstr(GAME_FIELD_HEIGHT - 6, GAME_FIELD_WIDTH * 2 * 1.3, str_record);
}

//
void print_next_figure(GameInfo_t CurrentState) {
  for (int i = 0; i < BLOCK_SIZE + 2; i++) {
    for (int j = 0; j < BLOCK_SIZE * 2 + 4; j++) {
      mvaddch(NEXT_FIGURE_Y + i, NEXT_FIGURE_X + j, ' ');
    }
  }

  mvprintw(NEXT_FIGURE_Y - 2, NEXT_FIGURE_X + 2, "NEXT");

  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      if (CurrentState.next[i][j]) {
        int y_pos = NEXT_FIGURE_Y + 1 + i;
        int x_pos = NEXT_FIGURE_X + 3 + j * 2;

        mvaddch(y_pos, x_pos, ' ' | A_REVERSE);
        mvaddch(y_pos, x_pos + 1, ' ' | A_REVERSE);
      }
    }
  }
}