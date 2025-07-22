#ifndef FRONTEND_H
#define FRONTEND_H
//
#define NEXT_FIGURE_X (GAME_FIELD_WIDTH * 2 + 10)  // Правее основного поля
#define NEXT_FIGURE_Y 2                            // Несколько строк от верха
#define NO_ACTION -1
//
#include <ncurses.h>

#include "../../common/common.h"

void initialize_ncurses();
void render(GameInfo_t CurrentState);
void render_game_field(int** filed);
void render_cell(int row, int col, bool is_filled);
void render_game_status(GameInfo_t CurrentState);
//
void print_next_figure(GameInfo_t CurrentState);
//

#endif