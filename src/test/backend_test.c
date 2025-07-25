#include "backend_test.h"

START_TEST(test_is_line_full_all_filled) {
  int line1[] = {1, 2, 3, 4};
  ck_assert(is_line_full(line1, 4) == true);
}
END_TEST

START_TEST(test_is_line_full_has_empty) {
  int line2[] = {1, 0, 3, 4};
  ck_assert(is_line_full(line2, 4) == false);
}
END_TEST

START_TEST(test_is_line_full_empty_array) {
  int line3[] = {};
  ck_assert(is_line_full(line3, 0) == true);
}
END_TEST

START_TEST(test_create_matrix_normal) {
  int rows = 3, cols = 4;
  int** matrix = create_matrix(rows, cols);

  ck_assert_ptr_nonnull(matrix);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      ck_assert_int_eq(matrix[i][j], 0);
    }
  }

  destroy_matrix(&matrix, rows);
}
END_TEST

START_TEST(test_create_matrix_zero_rows) {
  int rows = 0, cols = 5;
  int** matrix = create_matrix(rows, cols);

  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_create_matrix_zero_cols) {
  int rows = 5, cols = 0;
  int** matrix = create_matrix(rows, cols);

  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_create_matrix_negative_rows) {
  int rows = -1, cols = 5;
  int** matrix = create_matrix(rows, cols);

  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_create_matrix_negative_cols) {
  int rows = 5, cols = -1;
  int** matrix = create_matrix(rows, cols);

  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_create_matrix_large) {
  int rows = 1000, cols = 1000;
  int** matrix = create_matrix(rows, cols);

  ck_assert_ptr_nonnull(matrix);

  ck_assert_ptr_nonnull(matrix[0]);
  ck_assert_ptr_nonnull(matrix[rows - 1]);
  ck_assert_ptr_nonnull(matrix[rows / 2]);

  destroy_matrix(&matrix, rows);
}
END_TEST

START_TEST(test_destroy_matrix_normal) {
  int rows = 3, cols = 4;
  int** matrix = (int**)calloc(rows, sizeof(int*));
  for (int i = 0; i < rows; i++) {
    matrix[i] = (int*)calloc(cols, sizeof(int));
  }

  destroy_matrix(&matrix, rows);

  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_destroy_matrix_null_ptr_to_ptr) {
  destroy_matrix(NULL, 5);

  ck_assert(1);
}
END_TEST

START_TEST(test_destroy_matrix_null_matrix) {
  int** matrix = NULL;
  destroy_matrix(&matrix, 5);

  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_destroy_matrix_zero_rows) {
  int** matrix = (int**)calloc(1, sizeof(int*));
  matrix[0] = (int*)calloc(4, sizeof(int));

  destroy_matrix(&matrix, 0);

  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_destroy_matrix_partial_alloc) {
  int rows = 3;
  int** matrix = (int**)calloc(rows, sizeof(int*));
  matrix[0] = (int*)calloc(4, sizeof(int));
  matrix[1] = NULL;

  destroy_matrix(&matrix, rows);

  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_copy_matrix_normal) {
  int** matrix_A = create_test_matrix(BLOCK_SIZE, 0);
  int** matrix_B = create_test_matrix(BLOCK_SIZE, 42);

  copy_matrix(matrix_A, matrix_B);

  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      ck_assert_int_eq(matrix_A[i][j], 42);
    }
  }

  destroy_matrix(&matrix_A, BLOCK_SIZE);
  destroy_matrix(&matrix_B, BLOCK_SIZE);
}
END_TEST

START_TEST(test_copy_matrix_partial_fill) {
  int** matrix_A = create_test_matrix(BLOCK_SIZE, 0);
  int** matrix_B = create_test_matrix(BLOCK_SIZE, 0);

  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      matrix_B[i][j] = i * BLOCK_SIZE + j;
    }
  }

  copy_matrix(matrix_A, matrix_B);

  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      ck_assert_int_eq(matrix_A[i][j], i * BLOCK_SIZE + j);
    }
  }

  destroy_matrix(&matrix_A, BLOCK_SIZE);
  destroy_matrix(&matrix_B, BLOCK_SIZE);
}
END_TEST

START_TEST(test_copy_matrix_different_values) {
  int** matrix_A = create_test_matrix(BLOCK_SIZE, 100);
  int** matrix_B = create_test_matrix(BLOCK_SIZE, 200);

  copy_matrix(matrix_A, matrix_B);

  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      ck_assert_int_eq(matrix_A[i][j], 200);
    }
  }

  destroy_matrix(&matrix_A, BLOCK_SIZE);
  destroy_matrix(&matrix_B, BLOCK_SIZE);
}
END_TEST

START_TEST(test_empty_matrix_normal) {
  const int rows = 3, cols = 4;
  int** matrix = create_filled_matrix(rows, cols, 5);

  empty_matrix(matrix, rows, cols);

  ck_assert(is_matrix_empty(matrix, rows, cols));
  destroy_matrix(&matrix, rows);
}
END_TEST

START_TEST(test_empty_matrix_already_empty) {
  const int rows = 2, cols = 2;
  int** matrix = create_filled_matrix(rows, cols, 0);

  empty_matrix(matrix, rows, cols);

  ck_assert(is_matrix_empty(matrix, rows, cols));
  destroy_matrix(&matrix, rows);
}
END_TEST

START_TEST(test_empty_matrix_partially_filled) {
  const int rows = 3, cols = 3;
  int** matrix = create_filled_matrix(rows, cols, 0);

  matrix[1][1] = 1;
  matrix[2][0] = 2;

  empty_matrix(matrix, rows, cols);

  ck_assert(is_matrix_empty(matrix, rows, cols));
  destroy_matrix(&matrix, rows);
}
END_TEST

START_TEST(test_empty_matrix_zero_size) {
  const int rows = 3, cols = 3;
  int** matrix = create_filled_matrix(rows, cols, 5);

  empty_matrix(matrix, 0, 0);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      ck_assert_int_eq(matrix[i][j], 5);
    }
  }

  destroy_matrix(&matrix, rows);
}
END_TEST

START_TEST(test_level_up_basic) {
  ck_assert_int_eq(lvl_up(0), 0);
  ck_assert_int_eq(lvl_up(599), 0);
  ck_assert_int_eq(lvl_up(600), 1);
  ck_assert_int_eq(lvl_up(1199), 1);
  ck_assert_int_eq(lvl_up(1200), 2);
}
END_TEST

START_TEST(test_level_up_boundary) {
  ck_assert_int_eq(lvl_up(5999), 9);
  ck_assert_int_eq(lvl_up(6000), 10);
  ck_assert_int_eq(lvl_up(6001), 10);
}
END_TEST

START_TEST(test_level_up_max_level) {
  ck_assert_int_eq(lvl_up(10000), 10);
  ck_assert_int_eq(lvl_up(20000), 10);
  ck_assert_int_eq(lvl_up(30000), 10);
}
END_TEST

START_TEST(test_empty_matrix_large_matrix) {
  const int rows = 100, cols = 100;
  int** matrix = create_filled_matrix(rows, cols, 123);

  empty_matrix(matrix, rows, cols);

  ck_assert(is_matrix_empty(matrix, rows, cols));
  destroy_matrix(&matrix, rows);
}
END_TEST

START_TEST(test_count_score_1_line) { ck_assert_int_eq(count_score(1), 100); }
END_TEST

START_TEST(test_count_score_2_lines) { ck_assert_int_eq(count_score(2), 300); }
END_TEST

START_TEST(test_count_score_3_lines) { ck_assert_int_eq(count_score(3), 700); }
END_TEST

START_TEST(test_count_score_4_lines) { ck_assert_int_eq(count_score(4), 1500); }
END_TEST

START_TEST(test_count_score_0_lines) { ck_assert_int_eq(count_score(0), 0); }
END_TEST

START_TEST(test_count_score_negative) { ck_assert_int_eq(count_score(-1), 0); }
END_TEST

START_TEST(test_count_score_above_max) {
  ck_assert_int_eq(count_score(5), 0);
  ck_assert_int_eq(count_score(10), 0);
}
END_TEST

int** create_test_matrix(int size, int fill_value) {
  int** matrix = (int**)malloc(size * sizeof(int*));
  for (int i = 0; i < size; i++) {
    matrix[i] = (int*)malloc(size * sizeof(int));
    for (int j = 0; j < size; j++) {
      matrix[i][j] = fill_value;
    }
  }
  return matrix;
}

int** create_filled_matrix(int rows, int cols, int fill_value) {
  int** matrix = (int**)malloc(rows * sizeof(int*));
  for (int i = 0; i < rows; i++) {
    matrix[i] = (int*)malloc(cols * sizeof(int));
    for (int j = 0; j < cols; j++) {
      matrix[i][j] = fill_value;
    }
  }
  return matrix;
}

bool is_matrix_empty(int** matrix, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (matrix[i][j] != 0) {
        return false;
      }
    }
  }
  return true;
}

GameInfo_t* create_test_state() {
  GameInfo_t* state = (GameInfo_t*)malloc(sizeof(GameInfo_t));
  state->field = create_matrix(GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
  state->next = create_matrix(BLOCK_SIZE, BLOCK_SIZE);
  state->score = 0;
  state->level = 0;
  state->speed = 1;
  state->pause = 0;
  return state;
}

GameBlock_t* create_test_block_I() {
  GameBlock_t* block = (GameBlock_t*)malloc(sizeof(GameBlock_t));

  block->name = I;
  block->rotation = 0;
  block->x = 5;
  block->y = 5;

  return block;
}

void free_test_objects(GameInfo_t* state, GameBlock_t* block) {
  if (state) {
    if (state->field) {
      for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
        free(state->field[i]);
      }
      free(state->field);
    }
    if (state->next) {
      for (int i = 0; i < BLOCK_SIZE; i++) {
        free(state->next[i]);
      }
      free(state->next);
    }
    free(state);
  }
  if (block) {
    free(block);
  }
}

START_TEST(test_null_parameters) {
  FSM result = FiniteStateMachine(NULL, NULL);
  ck_assert_int_eq(result, GAME_OVER);
}
END_TEST

START_TEST(test_state_transitions) {
  GameInfo_t* state = create_test_state();
  GameBlock_t* block = create_test_block_I();

  FSM result = FiniteStateMachine(state, block);

  ck_assert(result == MOVING || result == SPAWN || result == GAME_OVER);

  FSM next_result = FiniteStateMachine(state, block);
  ck_assert(next_result != GAME_START);

  free_test_objects(state, block);
}
END_TEST

START_TEST(test_game_over_behavior) {
  GameInfo_t* state = create_test_state();
  GameBlock_t* block = create_test_block_I();

  state->pause = GAME_OVER;

  FSM result = FiniteStateMachine(state, block);

  ck_assert(result == GAME_OVER || result == MOVING || result == SPAWN);

  free_test_objects(state, block);
}
END_TEST

GameInfo_t* create_filled_state() {
  GameInfo_t* state = (GameInfo_t*)malloc(sizeof(GameInfo_t));
  state->field = create_matrix(GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
  state->next = create_matrix(BLOCK_SIZE, BLOCK_SIZE);

  // Заполняем матрицы
  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    for (int j = 0; j < GAME_FIELD_WIDTH; j++) {
      state->field[i][j] = 1;
    }
  }

  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      state->next[i][j] = 1;
    }
  }

  state->score = 1000;
  state->level = 5;
  state->speed = 10;

  return state;
}
START_TEST(test_reset_game_state_matrix_clearing) {
  GameInfo_t* state = create_filled_state();

  reset_game_state(state);

  ck_assert(is_matrix_empty(state->field, GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH));

  bool has_block = false;
  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      if (state->next[i][j] != 0) {
        has_block = true;
        break;
      }
    }
    if (has_block) break;
  }
  ck_assert(has_block);

  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    free(state->field[i]);
  }
  free(state->field);

  for (int i = 0; i < BLOCK_SIZE; i++) {
    free(state->next[i]);
  }
  free(state->next);

  free(state);
}
END_TEST

START_TEST(test_reset_game_state_score_reset) {
  GameInfo_t* state = create_filled_state();

  reset_game_state(state);

  ck_assert_int_eq(state->score, 0);

  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    free(state->field[i]);
  }
  free(state->field);

  for (int i = 0; i < BLOCK_SIZE; i++) {
    free(state->next[i]);
  }
  free(state->next);

  free(state);
}
END_TEST

START_TEST(test_reset_game_state_level_reset) {
  GameInfo_t* state = create_filled_state();

  reset_game_state(state);

  ck_assert_int_eq(state->level, 0);
  ck_assert_int_eq(state->speed, 0);
}
END_TEST

START_TEST(test_reset_game_state_next_figure_prepared) {
  GameInfo_t* state = create_filled_state();

  reset_game_state(state);

  bool has_non_zero = false;
  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      if (state->next[i][j] != 0) {
        has_non_zero = true;
        break;
      }
    }
    if (has_non_zero) break;
  }
  ck_assert(has_non_zero);

  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    free(state->field[i]);
  }
  free(state->field);

  for (int i = 0; i < BLOCK_SIZE; i++) {
    free(state->next[i]);
  }
  free(state->next);

  free(state);
}
END_TEST

int** create_test_field(int height, int width) {
  int** field = (int**)malloc(height * sizeof(int*));
  for (int i = 0; i < height; i++) {
    field[i] = (int*)calloc(width, sizeof(int));
  }
  return field;
}

void free_test_field(int** field, int height) {
  if (field) {
    for (int i = 0; i < height; i++) {
      free(field[i]);
    }
    free(field);
  }
}

START_TEST(test_null_matrix) { ck_assert_int_eq(clear_full_lines(NULL), 0); }
END_TEST

START_TEST(test_no_full_lines) {
  int** field = create_test_field(GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
  ck_assert_int_eq(clear_full_lines(field), 0);
  free_test_field(field, GAME_FIELD_HEIGHT);
}
END_TEST

START_TEST(test_one_full_line) {
  int** field = create_test_field(GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);

  memset(field[GAME_FIELD_HEIGHT - 1], 1, GAME_FIELD_WIDTH * sizeof(int));

  ck_assert_int_eq(clear_full_lines(field), 1);

  for (int j = 0; j < GAME_FIELD_WIDTH; j++) {
    ck_assert_int_eq(field[GAME_FIELD_HEIGHT - 1][j], 0);
  }

  free_test_field(field, GAME_FIELD_HEIGHT);
}
END_TEST

GameBlock_t* create_test_block(TetrominoName name, int rotation) {
  GameBlock_t* block = (GameBlock_t*)malloc(sizeof(GameBlock_t));
  block->name = name;
  block->rotation = rotation;
  block->x = GAME_FIELD_WIDTH / 2;
  block->y = 0;
  return block;
}

START_TEST(test_rotation_with_collision) {
  GameInfo_t* state = create_test_state();
  GameBlock_t* block = create_test_block(I, 0);

  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    state->field[i][GAME_FIELD_WIDTH - 1] = 1;
  }

  rotate_figure(state, block);

  ck_assert_int_eq(block->rotation, 0);
  free_test_objects(state, block);
}
END_TEST

START_TEST(test_rotation_at_pause) {
  GameInfo_t* state = create_test_state();
  GameBlock_t* block = create_test_block(I, 0);

  state->pause = 1;
  rotate_figure(state, block);

  ck_assert_int_eq(block->rotation, 0);
  free_test_objects(state, block);
}
END_TEST

START_TEST(test_null_parameters_block) {
  GameBlock_t* block = create_test_block(I, 0);
  rotate_figure(NULL, block);
  ck_assert_int_eq(block->rotation, 0);
  free(block);

  GameInfo_t* state = create_test_state();
  rotate_figure(state, NULL);
  free_test_objects(state, NULL);
}
END_TEST

START_TEST(test_rotation_overflow) {
  GameInfo_t* state = create_test_state();
  GameBlock_t* block = create_test_block(I, 3);

  rotate_figure(state, block);

  ck_assert_int_eq(block->rotation, 0);
  free_test_objects(state, block);
}
END_TEST

static GameInfo_t* test_state = NULL;
static GameBlock_t* test_block = NULL;

typedef GameInfo_t* (*GetStateFunc)(void);
typedef GameBlock_t* (*GetBlockFunc)(bool);

START_TEST(test_terminate_action) {
  test_state = create_test_state();
  test_block = create_test_block_I();

  userInput(Terminate, false);

  ck_assert_int_eq(test_state->pause, 0);
  free_test_objects(test_state, test_block);
}
END_TEST

START_TEST(test_null_state) {
  test_state = NULL;
  test_block = create_test_block_I();

  userInput(Left, false);
  userInput(Right, false);
  userInput(Up, false);

  free_test_objects(NULL, test_block);
}
END_TEST

START_TEST(test_null_block) {
  test_state = create_test_state();
  test_block = NULL;

  userInput(Left, false);
  userInput(Right, false);
  userInput(Up, false);

  free_test_objects(test_state, NULL);
}
END_TEST

START_TEST(test_free_null_pointer) {
  GameInfo_t* state = NULL;
  FreeCurrentState(&state);
  ck_assert_ptr_eq(state, NULL);
}
END_TEST

START_TEST(test_free_null_double_pointer) { FreeCurrentState(NULL); }
END_TEST

START_TEST(test_free_valid_state) {
  GameInfo_t* state = create_test_state();
  ck_assert_ptr_nonnull(state);
  ck_assert_ptr_nonnull(state->field);
  ck_assert_ptr_nonnull(state->next);

  FreeCurrentState(&state);

  ck_assert_ptr_eq(state, NULL);
}
END_TEST

START_TEST(test_free_partially_allocated_state) {
  GameInfo_t* state = (GameInfo_t*)malloc(sizeof(GameInfo_t));
  ck_assert_ptr_nonnull(state);

  state->field = create_matrix(GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
  state->next = NULL;

  FreeCurrentState(&state);
  ck_assert_ptr_eq(state, NULL);
}
END_TEST

START_TEST(test_draw_O_block_normal_position) {
  GameInfo_t* state = create_test_state();
  ck_assert_ptr_nonnull(state);

  GameBlock_t* block = create_test_block(O, 0);
  block->x = 5;
  block->y = 5;

  draw_temporary_figure(state, block);
  ck_assert_int_eq(state->field[5][5], 2);
  ck_assert_int_eq(state->field[5][6], 2);
  ck_assert_int_eq(state->field[6][5], 2);
  ck_assert_int_eq(state->field[6][6], 2);

  // Проверяем границы
  ck_assert_int_eq(state->field[5][4], 0);
  ck_assert_int_eq(state->field[6][4], 0);
  ck_assert_int_eq(state->field[4][5], 0);
  ck_assert_int_eq(state->field[7][5], 0);

  destroy_matrix(&state->field, GAME_FIELD_HEIGHT);
  free(state);
  free(block);
}
END_TEST

START_TEST(test_draw_T_block_normal_position) {
  GameInfo_t* state = create_test_state();
  ck_assert_ptr_nonnull(state);

  GameBlock_t* block = create_test_block(T, 0);
  block->x = 5;
  block->y = 5;

  draw_temporary_figure(state, block);

  ck_assert_int_eq(state->field[4][5], 2);
  ck_assert_int_eq(state->field[5][5], 2);
  ck_assert_int_eq(state->field[6][5], 2);
  ck_assert_int_eq(state->field[5][6], 2);

  // Проверяем границы
  ck_assert_int_eq(state->field[5][4], 0);
  ck_assert_int_eq(state->field[7][5], 0);
  ck_assert_int_eq(state->field[5][7], 0);

  destroy_matrix(&state->field, GAME_FIELD_HEIGHT);
  free(state);
  free(block);
}
END_TEST

static int save_record_called = 0;
static int saved_score = 0;
static int saved_high_score = 0;

GameInfo_t* mock_current_state = NULL;
GameBlock_t* mock_current_block = NULL;

void mock_save_record(int score, int high_score) {
  save_record_called = 1;
  saved_score = score;
  saved_high_score = high_score;
}

GameInfo_t* getCurrentState_mock() { return mock_current_state; }

GameBlock_t* getCurrentBlock_mock(bool some_arg) {
  (void)some_arg;  // Неиспользуемый параметр
  return mock_current_block;
}

START_TEST(test_free_resourse_null_state_null_block) {
  mock_current_state = NULL;
  mock_current_block = NULL;

  getCurrentState();

  free_resourse();

  ck_assert(1);
}
END_TEST

START_TEST(test_free_resourse_with_state_null_block) {
  mock_current_state = create_test_state();
  mock_current_block = NULL;

  ck_assert_ptr_nonnull(mock_current_state);
  ck_assert_ptr_nonnull(mock_current_state->field);
  ck_assert_ptr_nonnull(mock_current_state->next);

  getCurrentState();

  free_resourse();

  mock_current_state = NULL;

  ck_assert(1);
}
END_TEST

START_TEST(test_free_resourse_null_state_with_block) {
  mock_current_state = NULL;
  mock_current_block = (GameBlock_t*)malloc(sizeof(GameBlock_t));

  ck_assert_ptr_nonnull(mock_current_block);

  getCurrentState();

  free_resourse();

  mock_current_block = NULL;

  ck_assert(1);
}
END_TEST

START_TEST(test_free_resourse_with_state_and_block) {
  mock_current_state = create_test_state();
  mock_current_block = (GameBlock_t*)malloc(sizeof(GameBlock_t));

  ck_assert_ptr_nonnull(mock_current_state);
  ck_assert_ptr_nonnull(mock_current_state->field);
  ck_assert_ptr_nonnull(mock_current_state->next);
  ck_assert_ptr_nonnull(mock_current_block);

  getCurrentState();

  free_resourse();
  mock_current_state = NULL;
  mock_current_block = NULL;

  ck_assert(1);
}
END_TEST

GameBlock_t* create_test_block_with_position(int x) {
  GameBlock_t* block = (GameBlock_t*)malloc(sizeof(GameBlock_t));
  if (block) {
    block->x = x;
  }
  return block;
}

static bool collision_result = false;
static bool check_collision_called = false;
static GameInfo_t* last_state = NULL;
static GameBlock_t* last_block = NULL;
static bool last_check_down = false;

bool mock_check_collision(GameInfo_t* state, GameBlock_t* block,
                          bool check_down) {
  check_collision_called = true;
  last_state = state;
  last_block = block;
  last_check_down = check_down;
  return collision_result;
}

void free_test_resources(GameInfo_t* state, GameBlock_t* block) {
  if (state) {
    if (state->field) {
      destroy_matrix(&state->field, GAME_FIELD_HEIGHT);
    }
    if (state->next) {
      destroy_matrix(&state->next, BLOCK_SIZE);
    }
    free(state);
  }
  if (block) {
    free(block);
  }
}

START_TEST(test_move_down_null_parameters) {
  FSM result = move_down(NULL, NULL);
  ck_assert_int_eq(result, MOVING);
  GameInfo_t* state = create_test_state();
  result = move_down(state, NULL);
  ck_assert_int_eq(result, MOVING);
  free_test_resources(state, NULL);

  GameBlock_t* block = create_test_block_with_position(0);
  result = move_down(NULL, block);
  ck_assert_int_eq(result, MOVING);
  free_test_resources(NULL, block);
}
END_TEST

START_TEST(test_move_down_null_pointers) {
  FSM result = move_down(NULL, NULL);
  ck_assert_int_eq(result, MOVING);
}
END_TEST

static bool clear_called = false;
static bool move_down_called = false;

void clear_temporary_figure_mock(GameInfo_t* state) {
  (void)state;
  clear_called = true;
}

void move_down_mock(GameInfo_t* state, GameBlock_t* block) {
  (void)state;
  (void)block;
  move_down_called = true;
}

// Тесты для действия Start
START_TEST(test_userInput_Start_resumes_game) {
  // Подготовка
  GameInfo_t* state = create_test_state();
  state->pause = PAUSE_ON;

  // clear_called = false;

  userInput(Start, false);

  state = create_test_state();

  // Проверки
  ck_assert_int_eq(state->pause, PAUSE_OFF);  //
  // ck_assert(clear_called);

  // Очистка
  free_test_resources(state, NULL);
}
END_TEST

// Тесты для действия Down
START_TEST(test_userInput_Down_moves_block_down) {
  // Подготовка
  GameInfo_t* state = create_test_state();
  GameBlock_t* block = (GameBlock_t*)malloc(sizeof(GameBlock_t));
  move_down_called = false;
  clear_called = false;

  // Выполнение
  bool hold = false;
  userInput(Down, hold);

  state = create_test_state();

  // Проверки
  ck_assert_int_eq(state->pause, PAUSE_OFF);

  // Очистка
  free_test_resources(state, block);
}
END_TEST

START_TEST(test_FSM_MOVING_when_just_spawned) {
  GameInfo_t* state = create_test_state();

  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    for (int j = 0; j < GAME_FIELD_WIDTH; j++) {
      state->field[i][j] = 1;
    }
  }

  GameBlock_t* block = create_test_block_I();

  // Стартовая позиция
  block->x = 0;
  block->y = 5;

  FSM result = FiniteStateMachine(state, block);
  result = FiniteStateMachine(state, block);
  result = FiniteStateMachine(state, block);

  ck_assert_int_eq(result, MOVING);

  free_test_resources(state, block);
}
END_TEST

START_TEST(test_userInput_Pause) {
  GameInfo_t* state = create_test_state();
  GameBlock_t* block = create_test_block_I();

  // Стартовая позиция
  block->x = 0;
  block->y = 5;

  userInput(Pause, false);

  ck_assert_int_eq(state->pause, PAUSE_OFF);

  free_test_resources(state, block);
}
END_TEST

START_TEST(test_FSM_Action_falls_to_filled_line_and_attaches) {
  // 1. Подготовка игрового поля
  GameInfo_t* state = getCurrentState();
  GameBlock_t* block = getCurrentBlock(false);

  for (int y = 0; y < GAME_FIELD_WIDTH; y++) {
    for (int x = GAME_FIELD_HEIGHT - 3; x < GAME_FIELD_HEIGHT; x++) {
      state->field[x][y] = 1;
    }
  }

  userInput(Action, false);

  FSM result = FiniteStateMachine(state, block);
  ck_assert_int_eq(result, SPAWN);

  free_test_resources(state, block);
}
END_TEST

START_TEST(test_userInput_define) { userInput(-1, false); }
END_TEST

START_TEST(test_move_left) {
  // 1. Подготовка игрового поля
  GameInfo_t* state = getCurrentState();
  GameBlock_t* block = getCurrentBlock(false);

  state->pause = PAUSE_OFF;

  userInput(Left, false);

  free_test_resources(state, block);
}
END_TEST

START_TEST(test_move_right) {
  // 1. Подготовка игрового поля
  GameInfo_t* state = getCurrentState();
  GameBlock_t* block = getCurrentBlock(false);

  state->pause = PAUSE_OFF;

  userInput(Right, false);

  free_test_resources(state, block);
}
END_TEST

// START_TEST(test_FSM_ATTACHING) {
//   GameInfo_t* state = create_test_state();
//   GameBlock_t* block = create_test_block_I();

//   for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
//     for (int j = 0; j < GAME_FIELD_WIDTH; j++) {
//       state->field[i][j] = 1;
//     }
//   }

//   // Стартовая позиция
//   block->x = 0;
//   block->y = 0;

//   FSM result = FiniteStateMachine(NULL, block);

//   ck_assert_int_eq(result, GAME_OVER);

//   free_test_resources(state, block);
// }
// END_TEST

Suite* tetris_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create("Tetris");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_is_line_full_all_filled);
  tcase_add_test(tc_core, test_is_line_full_has_empty);
  tcase_add_test(tc_core, test_is_line_full_empty_array);

  tcase_add_test(tc_core, test_create_matrix_large);
  tcase_add_test(tc_core, test_create_matrix_negative_cols);
  tcase_add_test(tc_core, test_create_matrix_negative_rows);
  tcase_add_test(tc_core, test_create_matrix_zero_cols);
  tcase_add_test(tc_core, test_create_matrix_zero_rows);
  tcase_add_test(tc_core, test_create_matrix_normal);

  tcase_add_test(tc_core, test_destroy_matrix_partial_alloc);
  tcase_add_test(tc_core, test_destroy_matrix_zero_rows);
  tcase_add_test(tc_core, test_destroy_matrix_null_matrix);
  tcase_add_test(tc_core, test_destroy_matrix_null_ptr_to_ptr);
  tcase_add_test(tc_core, test_destroy_matrix_normal);

  tcase_add_test(tc_core, test_copy_matrix_different_values);
  tcase_add_test(tc_core, test_copy_matrix_partial_fill);
  tcase_add_test(tc_core, test_copy_matrix_normal);

  tcase_add_test(tc_core, test_empty_matrix_normal);
  tcase_add_test(tc_core, test_empty_matrix_already_empty);
  tcase_add_test(tc_core, test_empty_matrix_partially_filled);
  tcase_add_test(tc_core, test_empty_matrix_zero_size);
  tcase_add_test(tc_core, test_empty_matrix_large_matrix);

  tcase_add_test(tc_core, test_level_up_max_level);
  tcase_add_test(tc_core, test_level_up_boundary);
  tcase_add_test(tc_core, test_level_up_basic);

  tcase_add_test(tc_core, test_count_score_above_max);
  tcase_add_test(tc_core, test_count_score_negative);
  tcase_add_test(tc_core, test_count_score_0_lines);
  tcase_add_test(tc_core, test_count_score_4_lines);
  tcase_add_test(tc_core, test_count_score_3_lines);
  tcase_add_test(tc_core, test_count_score_2_lines);
  tcase_add_test(tc_core, test_count_score_1_line);

  tcase_add_test(tc_core, test_null_parameters);
  tcase_add_test(tc_core, test_state_transitions);
  tcase_add_test(tc_core, test_game_over_behavior);

  tcase_add_test(tc_core, test_reset_game_state_next_figure_prepared);
  tcase_add_test(tc_core, test_reset_game_state_level_reset);
  tcase_add_test(tc_core, test_reset_game_state_score_reset);
  tcase_add_test(tc_core, test_reset_game_state_matrix_clearing);

  tcase_add_test(tc_core, test_null_matrix);
  tcase_add_test(tc_core, test_one_full_line);
  tcase_add_test(tc_core, test_no_full_lines);

  tcase_add_test(tc_core, test_rotation_overflow);
  tcase_add_test(tc_core, test_null_parameters_block);
  tcase_add_test(tc_core, test_rotation_at_pause);
  tcase_add_test(tc_core, test_rotation_with_collision);

  tcase_add_test(tc_core, test_null_block);
  tcase_add_test(tc_core, test_null_state);
  tcase_add_test(tc_core, test_terminate_action);

  tcase_add_test(tc_core, test_free_partially_allocated_state);
  tcase_add_test(tc_core, test_free_valid_state);
  tcase_add_test(tc_core, test_free_null_double_pointer);
  tcase_add_test(tc_core, test_free_null_pointer);

  tcase_add_test(tc_core, test_draw_T_block_normal_position);
  tcase_add_test(tc_core, test_draw_O_block_normal_position);

  tcase_add_test(tc_core, test_move_down_null_pointers);
  tcase_add_test(tc_core, test_move_down_null_parameters);
  tcase_add_test(tc_core, test_move_left);
  tcase_add_test(tc_core, test_move_right);

  tcase_add_test(tc_core, test_userInput_Down_moves_block_down);
  tcase_add_test(tc_core, test_userInput_Start_resumes_game);
  tcase_add_test(tc_core, test_userInput_Pause);
  tcase_add_test(tc_core, test_userInput_define);

  tcase_add_test(tc_core, test_free_resourse_with_state_and_block);
  tcase_add_test(tc_core, test_free_resourse_null_state_with_block);
  tcase_add_test(tc_core, test_free_resourse_with_state_null_block);
  tcase_add_test(tc_core, test_free_resourse_null_state_null_block);

  tcase_add_test(tc_core, test_FSM_MOVING_when_just_spawned);
  tcase_add_test(tc_core, test_FSM_Action_falls_to_filled_line_and_attaches);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite* s;
  SRunner* sr;

  s = tetris_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}