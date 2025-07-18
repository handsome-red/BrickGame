#ifndef FRONTEND_H
#define FRONTEND_H
//
#define NEXT_FIGURE_X (GAME_FIELD_WIDTH * 2 + 6)  // Правее основного поля
#define NEXT_FIGURE_Y 2                           // Несколько строк от верха
//
#include <ncurses.h>

#include "../../common/common.h"

void init_ncurses();
void print_game_field(GameInfo_t CurrentState);
void print_game_info(GameInfo_t CurrentState);
//
void print_next_figure(GameInfo_t CurrentState);
//

#endif