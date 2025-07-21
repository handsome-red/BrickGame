#include "main.h"

#include "brick_game/tetris/backend.h"
#include "gui/cli/frontend.h"

int main() {
  game();
  endwin();
  return 0;
}

void game() {
  srand(time(NULL));
  init_ncurses();
  GameInfo_t CurrentState = {0};
  bool cycle = game_over(false);

  // do {
  //   CurrentState = updateCurrentState();
  //   if (CurrentState.field != NULL) {
  //     print_game_field(CurrentState);
  //     print_game_info(CurrentState);
  //   }

  // } while (CurrentState.field != NULL);

  while (!cycle) {
    userInput(readInput(), 1);
    cycle = game_over(false);
    if (cycle == false) {
      CurrentState = updateCurrentState();
      // if (CurrentState.field == NULL || CurrentState.pause == 2) {
      //   break;
      // }
      print_game_field(CurrentState);
      print_game_info(CurrentState);
    }
    refresh();
  }
}