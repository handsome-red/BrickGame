#ifndef BACKEND_TEST_H
#define BACKEND_TEST_H

#define PAUSE_ON 1
#include <check.h>

#include "../brick_game/tetris/backend.h"
#include "../common/common.h"

int** create_test_matrix(int size, int fill_value);
bool is_matrix_empty(int** matrix, int rows, int cols);
int** create_filled_matrix(int rows, int cols, int fill_value);
GameInfo_t* create_test_state();

// Объявляем типы для указателей на функции
typedef GameInfo_t* (*GetStateFunc)(void);
typedef GameBlock_t* (*GetBlockFunc)(bool);

GameBlock_t* create_test_block_I();
void free_test_objects(GameInfo_t* state, GameBlock_t* block);
void free_test_resources();

#endif

// #pragma once

// #include <check.h>

// #include "../brick_game/tetris/backend.h"
// #include "../common/common.h"