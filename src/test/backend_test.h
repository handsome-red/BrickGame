#ifndef BACKEND_TEST_H
#define BACKEND_TEST_H

#include "../brick_game/tetris/backend.h"
#include "../common/common.h"

// Function declarations
void init_game_state(GameState_t* state);
void fill_matrix(int** matrix, int value, int height, int width);
void setCurrentState(GameState_t* state);
void setCurrentBlock(GameBlock_t* block);
void set_high_score_mock(int score);
void set_check_collision_mock(bool result);
void set_foo_attaching_mock(FSM result);
void set_clear_full_lines_mock(int lines);
void set_count_score_mock(int score);
void set_lvl_up_mock(int level);
void set_update_record_mock(int record);

// Constants
#define PAUSE_ON 1
#define PAUSE_OFF 0

// Mock variables (if not already declared)
extern bool collision_mock_result;
extern FSM foo_attaching_mock_result;
extern int cleared_lines_mock;
extern int score_mock;
extern int level_mock;
extern int record_mock;
extern GameBlock_t* mock_current_block;

#endif