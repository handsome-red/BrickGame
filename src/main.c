#include "main.h"

#include "brick_game/tetris/backend.h"
#include "gui/cli/frontend.h"

int main() {
  game();
  return 0;
}

void game() {
  srand(time(NULL));
  initialize_ncurses();
  GameInfo_t CurrentState = updateCurrentState();

  do {
    userInput(readInput(), true);
    CurrentState = updateCurrentState();
    render(CurrentState);
  } while (CurrentState.pause != STOP);

  free_resourse();
  endwin();
}