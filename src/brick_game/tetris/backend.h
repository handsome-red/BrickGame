#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../common/common.h"
#define PAUSE_OFF 0
#define STOP -1
#define PREVIEW -2

typedef enum { GAME_START, MOVING, SPAWN, ATTACHING, GAME_OVER } FSM;

typedef enum { I, J, L, O, S, T, Z } TetrominoName;

typedef struct {
  int x, y;
} TetrominoBlock;

typedef struct {
  TetrominoBlock blocks[4];
} TetrominoState;

typedef struct {
  TetrominoName name;     // Название фигуры
  int rotation;           // Состояние поворота
  int x, y;               // Координаты якоря на поле
  TetrominoState coords;  // Координаты блоков вокруг фигур
} GameBlock_t;

void destroy_matrix(int*** matrix_ptr, int rows);
int** create_matrix(int rows, int cols);
void userInput(UserAction_t action, bool hold);
// GameInfo_t get_game_info(); //
GameInfo_t updateCurrentState();
GameInfo_t* getCurrentState();
// GameBlock_t updateCurrentBlock();
UserAction_t readInput();
TetrominoState blockState(int BlockType, int BlockState);
void copy_next_to_block(GameInfo_t* CurrentState, GameBlock_t* block);
int GameTimer(int level, int pause);
FSM FiniteStateMachine(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
void FreeCurrentState(GameInfo_t** CurrentState);
void empty_matrix(int** matrix, int row, int col);
void prepare_next_figure(GameInfo_t* CurrentState);
bool check_collision(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock,
                     bool predict);
void copy_matrix(int** matrix_A, int** matrix_B);

FSM foo_attaching(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
FSM foo_detachment(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);

FSM move_down(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
FSM move_left(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
FSM move_right(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
FSM fall_down(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);

void rotate_figure(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);

int clear_full_lines(GameInfo_t* state);
bool is_line_full(const int* line, int width);
void shift_lines_down(GameInfo_t* state, int from_row);
int count_score(int lines);
int lvl_up(int score);
void save_record(int score, int record);
int update_record(int score, int record);
int load_high_score();

void draw_temporary_figure(GameInfo_t* state, GameBlock_t* block);
void clear_temporary_figure(GameInfo_t* state);
GameBlock_t* getCurrentBlock(bool reset);

FSM on_game_over(GameInfo_t* CurrentState);
FSM on_game_start(GameInfo_t* CurrentState);
FSM on_game_spawn(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
FSM on_attaching(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);

void free_resourse();
void reset_game_state(GameInfo_t* state);

#endif