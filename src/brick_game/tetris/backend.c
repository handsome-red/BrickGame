#include "./backend.h"

FSM FiniteStateMachine(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  static FSM game_state = GAME_START;

  if (!CurrentState || !CurrentBlock) {
    return GAME_OVER;
  }

  switch (game_state) {
    case GAME_START:
      game_state = on_game_start(CurrentState);
      break;
    case MOVING:
      game_state = move_down(CurrentState, CurrentBlock);
      break;
    case SPAWN:
      game_state = on_game_spawn(CurrentState, CurrentBlock);
      break;
    case ATTACHING:
      game_state = on_attaching(CurrentState, CurrentBlock);
      break;
    case GAME_OVER:
      game_state = on_game_over(CurrentState);
      break;
    default:
      break;
  }
  return game_state;
}

void reset_game_state(GameInfo_t* state) {
  empty_matrix(state->field, GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
  empty_matrix(state->next, BLOCK_SIZE, BLOCK_SIZE);

  state->score = 0;
  state->level = 0;
  state->speed = state->level;

  prepare_next_figure(state);
}

void userInput(UserAction_t action, bool hold) {
  (void)hold;

  GameInfo_t* CurrentState = getCurrentState();
  GameBlock_t* CurrentBlock = getCurrentBlock(false);

  if (CurrentState) {
    clear_temporary_figure(CurrentState);
  }

  switch (action) {
    case Start:
      CurrentState->pause = PAUSE_OFF;
      break;
    case Pause:
      CurrentState->pause = !CurrentState->pause;
      break;
    case Terminate:
      CurrentState->pause = STOP;
      save_record(CurrentState->score, CurrentState->high_score);
      break;
    case Left:
      move_left(CurrentState, CurrentBlock);
      break;
    case Right:
      move_right(CurrentState, CurrentBlock);
      break;
    case Up:
      rotate_figure(CurrentState, CurrentBlock);
      break;
    case Down:
      hold = true;
      move_down(CurrentState, CurrentBlock);
      break;
    case Action:
      fall_down(CurrentState, CurrentBlock);
      break;
    default:
      break;
  }
}

GameInfo_t* getCurrentState() {
  static GameInfo_t* CurrentState = NULL;
  static bool initialized = false;

  if (!initialized) {
    CurrentState = (GameInfo_t*)malloc(sizeof(GameInfo_t));
    if (!CurrentState) return NULL;

    CurrentState->field = create_matrix(GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
    CurrentState->next = create_matrix(BLOCK_SIZE, BLOCK_SIZE);

    if (!CurrentState->field || !CurrentState->next) {
      FreeCurrentState(&CurrentState);
      return NULL;
    }

    CurrentState->score = 0;
    CurrentState->high_score = load_high_score();
    CurrentState->level = 0;
    CurrentState->speed = CurrentState->level;
    CurrentState->pause = PREVIEW;

    initialized = true;
  }

  GameBlock_t* CurrentBlock = getCurrentBlock(false);
  if (CurrentBlock) {
    clear_temporary_figure(CurrentState);

    if (GameTimer(CurrentState->level, CurrentState->pause)) {
      FiniteStateMachine(CurrentState, CurrentBlock);
    }

    draw_temporary_figure(CurrentState, CurrentBlock);
  }

  return CurrentState;
}

void free_resourse() {
  GameInfo_t* CurrentState = getCurrentState();
  GameBlock_t* CurrentBlock = getCurrentBlock(false);

  if (CurrentState) {
    destroy_matrix(&CurrentState->field, GAME_FIELD_HEIGHT);
    destroy_matrix(&CurrentState->next, BLOCK_SIZE);
    free(CurrentState);
  }

  if (CurrentBlock) {
    free(CurrentBlock);
  }
}

FSM on_attaching(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  FSM game_state = SPAWN;
  if (!check_collision(CurrentState, CurrentBlock, true)) {
    game_state = MOVING;
  } else {
    game_state = foo_attaching(CurrentState, CurrentBlock);
    CurrentState->score += count_score(clear_full_lines(CurrentState->field));
    CurrentState->level = lvl_up(CurrentState->score);
    CurrentState->high_score =
        update_record(CurrentState->score, CurrentState->high_score);
  }
  return game_state;
}

FSM on_game_spawn(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  copy_next_to_block(CurrentState, CurrentBlock);
  prepare_next_figure(CurrentState);
  return MOVING;
}

FSM on_game_start(GameInfo_t* CurrentState) {
  empty_matrix(CurrentState->field, GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
  empty_matrix(CurrentState->next, BLOCK_SIZE, BLOCK_SIZE);
  prepare_next_figure(CurrentState);
  CurrentState->level = 0;
  CurrentState->score = 0;
  return SPAWN;
}

FSM on_game_over(GameInfo_t* CurrentState) {
  if (CurrentState->score >= CurrentState->high_score) {
    // CurrentState->high_score = CurrentState->score;
    save_record(CurrentState->score, CurrentState->high_score);
  }

  CurrentState->pause = PREVIEW;

  return GAME_START;
}

int update_record(int score, int record) {
  return score > record ? score : record;
}

void save_record(int score, int record) {
  if (score == record) {
    FILE* file = fopen("./high_score.txt", "w");
    if (file) {
      fprintf(file, "%d", score);
      fclose(file);
    }
  }
}

int lvl_up(int score) {
  int lvl = score / 600;
  if (lvl > 10) {
    lvl = 10;
  }
  return lvl;
}

// Переписать все функции чтобы они возвращали состояние конечного автомата
// Изменение скорости
int count_score(int lines) {
  int score = 0;

  switch (lines) {
    case 1:
      score = 100;
      break;
    case 2:
      score = 300;
      break;
    case 3:
      score = 700;
      break;
    case 4:
      score = 1500;
      break;
  }
  return score;
}

FSM move_down(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  FSM state = MOVING;

  if (CurrentState && CurrentBlock) {
    if (!CurrentState->pause) {
      if (!check_collision(CurrentState, CurrentBlock, true)) {
        CurrentBlock->x++;
      } else {
        state = ATTACHING;
      }
    }
  }
  return state;
}

bool check_collision(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock,
                     bool predict) {
  TetrominoState coords =
      blockState(CurrentBlock->name, CurrentBlock->rotation);

  for (int i = 0; i < 4; i++) {
    int world_x = CurrentBlock->x + coords.blocks[i].x + predict;
    int world_y = CurrentBlock->y + coords.blocks[i].y;

    if (world_x >= GAME_FIELD_HEIGHT || world_y < 0 ||
        world_y >= GAME_FIELD_WIDTH ||
        (world_x >= 0 && CurrentState->field[world_x][world_y] > 0)) {
      return true;
    }
  }
  return false;
}

FSM fall_down(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  FSM state = move_down(CurrentState, CurrentBlock);
  int i = 0;
  while (state == MOVING && i < GAME_FIELD_HEIGHT) {
    state = move_down(CurrentState, CurrentBlock);
    i++;
  }
  return state;
}

FSM move_left(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  FSM state = MOVING;
  if (!CurrentState->pause) {
    CurrentBlock->y--;
    if (check_collision(CurrentState, CurrentBlock, false)) {
      CurrentBlock->y++;
    }
  }

  return state;
}

FSM move_right(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  FSM state = MOVING;
  if (!CurrentState->pause) {
    CurrentBlock->y++;
    if (check_collision(CurrentState, CurrentBlock, false)) {
      CurrentBlock->y--;
    }
  }

  return state;
}

TetrominoState blockState(int BlockType, int BlockState) {
  static const TetrominoState TETRIMINOS[7][4] = {
      [I] = {{{{-1, 0}, {0, 0}, {1, 0}, {2, 0}}},
             {{{0, -1}, {0, 0}, {0, 1}, {0, 2}}},
             {{{-1, 0}, {0, 0}, {1, 0}, {2, 0}}},   //
             {{{0, -1}, {0, 0}, {0, 1}, {0, 2}}}},  //
      [J] = {{{{-1, -1}, {-1, 0}, {0, 0}, {1, 0}}},
             {{{0, -1}, {0, 0}, {0, 1}, {-1, 1}}},
             {{{-1, 0}, {0, 0}, {1, 0}, {1, 1}}},
             {{{0, -1}, {0, 0}, {0, 1}, {1, -1}}}},
      [L] = {{{{1, -1}, {-1, 0}, {0, 0}, {1, 0}}},
             {{{-1, -1}, {0, -1}, {0, 0}, {0, 1}}},
             {{{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}},
             {{{0, -1}, {0, 0}, {0, 1}, {1, 1}}}},
      [O] = {{{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
             {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},   //
             {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},   //
             {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}}},  //

      [S] = {{{{0, 0}, {1, 0}, {-1, 1}, {0, 1}}},
             {{{0, -1}, {0, 0}, {1, 0}, {1, 1}}},
             {{{0, 0}, {1, 0}, {-1, 1}, {0, 1}}},   //
             {{{0, -1}, {0, 0}, {1, 0}, {1, 1}}}},  //
      [T] = {{{{-1, 0}, {0, 0}, {1, 0}, {0, 1}}},
             {{{0, -1}, {0, 0}, {0, 1}, {1, 0}}},
             {{{-1, 0}, {0, 0}, {1, 0}, {0, -1}}},
             {{{0, -1}, {0, 0}, {0, 1}, {-1, 0}}}},
      [Z] = {{{{-1, 0}, {0, 0}, {0, 1}, {1, 1}}},
             {{{1, -1}, {0, 0}, {1, 0}, {0, 1}}},
             {{{-1, 0}, {0, 0}, {0, 1}, {1, 1}}},    //
             {{{1, -1}, {0, 0}, {1, 0}, {0, 1}}}}};  //
  return TETRIMINOS[BlockType][BlockState];
}

int GameTimer(int level, int pause) {
  int flag = 0;
  static struct timespec lastTime = {0, 0};
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  int interval = 1000 / (1 + level * 0.5);
  if (interval < 50) interval = 50;

  double interval_check = (now.tv_sec - lastTime.tv_sec) * 1000 +
                          (now.tv_nsec - lastTime.tv_nsec) / 1000000;
  if (interval_check >= interval && pause == 0) {
    flag = 1;
    lastTime = now;
  }

  return flag;
}

GameInfo_t updateCurrentState() {
  GameInfo_t* CurrentState = getCurrentState();
  return *CurrentState;
}

GameBlock_t* getCurrentBlock(bool reset) {
  static GameBlock_t* CurrentBlock = NULL;
  if (reset && CurrentBlock) {
    free(CurrentBlock);
    CurrentBlock = NULL;
    return NULL;
  }
  if (NULL == CurrentBlock) {
    CurrentBlock = (GameBlock_t*)malloc(sizeof(GameBlock_t));
    CurrentBlock->name = -1;
    CurrentBlock->rotation = 0;
    CurrentBlock->x = 0;
    CurrentBlock->y = GAME_FIELD_WIDTH / 2 - 2;
    for (int i = 0; i < 4; i++) {
      CurrentBlock->coords.blocks[i].x = 0;
      CurrentBlock->coords.blocks[i].y = 0;
    }
  }
  return CurrentBlock;
}

int load_high_score() {
  int high_score = 0;
  FILE* file = fopen("./high_score.txt", "r");

  if (file) {
    if (fscanf(file, "%d", &high_score) != 1) {
      high_score = 0;
    }
    fclose(file);
  }

  return high_score;
}

void clear_temporary_figure(GameInfo_t* state) {
  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    for (int j = 0; j < GAME_FIELD_WIDTH; j++) {
      if (state->field[i][j] == 2) {
        state->field[i][j] = 0;
      }
    }
  }
}

void draw_temporary_figure(GameInfo_t* state, GameBlock_t* block) {
  TetrominoState coords = blockState(block->name, block->rotation);
  for (int i = 0; i < 4; i++) {
    int x = block->x + coords.blocks[i].x;
    int y = block->y + coords.blocks[i].y;

    if (x >= 0 && x < GAME_FIELD_HEIGHT && y >= 0 && y < GAME_FIELD_WIDTH) {
      if (state->field[x][y] == 0 || state->field[x][y] == 2) {
        state->field[x][y] = 2;
      }
    }
  }
}

// Модифицированная функция прикрепления
FSM foo_attaching(GameInfo_t* state, GameBlock_t* block) {
  TetrominoState coords = blockState(block->name, block->rotation);
  bool can_spawn = true;

  for (int y = GAME_FIELD_WIDTH / 2 - 2; y < GAME_FIELD_WIDTH / 2 + 1; y++) {
    if (state->field[0][y] != 0) {
      can_spawn = false;
      break;
    }
    if (!can_spawn) break;
  }

  for (int i = 0; i < 4; i++) {
    int x = block->x + coords.blocks[i].x;
    int y = block->y + coords.blocks[i].y;

    if (x >= 0 && x < GAME_FIELD_HEIGHT && y >= 0 && y < GAME_FIELD_WIDTH) {
      state->field[x][y] = 1;
    }
  }

  return can_spawn ? SPAWN : GAME_OVER;
}

void copy_next_to_block(GameInfo_t* CurrentState, GameBlock_t* block) {
  if (!CurrentState || !CurrentState->next || !block) {
    return;
  }

  int figure_type = STOP;
  for (int i = 0; i < BLOCK_SIZE && figure_type == -1; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      if (CurrentState->next[i][j] != 0) {
        figure_type = CurrentState->next[i][j] - 1;
        break;
      }
    }
  }

  if (figure_type < 0 || figure_type > Z) {
    return;
  }

  block->name = (TetrominoName)figure_type;
  block->rotation = 0;
  block->x = -1;
  block->y = GAME_FIELD_WIDTH / 2 - 1;

  TetrominoState state = blockState(figure_type, 0);
  for (int i = 0; i < 4; i++) {
    block->coords.blocks[i] = state.blocks[i];
  }
}

void prepare_next_figure(GameInfo_t* CurrentState) {
  empty_matrix(CurrentState->next, BLOCK_SIZE, BLOCK_SIZE);
  int next_type = rand() % 7;
  TetrominoState next = blockState(next_type, 0);

  int min_x = 0, min_y = 0;
  for (int i = 0; i < 4; i++) {
    if (next.blocks[i].x < min_x) min_x = next.blocks[i].x;
    if (next.blocks[i].y < min_y) min_y = next.blocks[i].y;
  }
  int offset_x = -min_x;
  int offset_y = -min_y;

  for (int i = 0; i < 4; i++) {
    int col = next.blocks[i].x + offset_x;
    int row = next.blocks[i].y + offset_y;

    if (row >= 0 && row < BLOCK_SIZE && col >= 0 && col < BLOCK_SIZE) {
      CurrentState->next[col][row] = next_type + 1;
    }
  }
}

void FreeCurrentState(GameInfo_t** CurrentState) {
  if (CurrentState == NULL || *CurrentState == NULL) {
    return;
  }

  GameInfo_t* state = *CurrentState;

  destroy_matrix(&state->field, GAME_FIELD_HEIGHT);
  destroy_matrix(&state->next, BLOCK_SIZE);

  free(state);
  *CurrentState = NULL;
}

void destroy_matrix(int*** matrix_ptr, int rows) {
  if (matrix_ptr == NULL || *matrix_ptr == NULL) {
    return;
  }

  int** matrix = *matrix_ptr;

  for (int i = 0; i < rows; i++) {
    free(matrix[i]);
  }

  free(matrix);

  *matrix_ptr = NULL;
}

int** create_matrix(int rows, int cols) {
  if (rows <= 0 || cols <= 0) {
    return NULL;
  }

  int** matrix = (int**)calloc(rows, sizeof(int*));
  if (matrix == NULL) {
    return NULL;
  }

  for (int i = 0; i < rows; i++) {
    matrix[i] = (int*)calloc(cols, sizeof(int));

    if (matrix[i] == NULL) {
      for (int j = 0; j < i; j++) {
        free(matrix[j]);
      }
      free(matrix);
      return NULL;
    }
  }

  return matrix;
}

void copy_matrix(int** matrix_A, int** matrix_B) {
  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      matrix_A[i][j] = matrix_B[i][j];
    }
  }
}

void empty_matrix(int** matrix, int row, int col) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      matrix[i][j] = 0;
    }
  }
}

int clear_full_lines(int** matrix) {
  int lines_cleared = 0;

  if (NULL != matrix) {
    for (int row = GAME_FIELD_HEIGHT - 1; row >= 0; row--) {
      if (is_line_full(matrix[row], GAME_FIELD_WIDTH)) {
        shift_lines_down(matrix, row);
        row++;
        lines_cleared++;
      }
    }
  }

  return lines_cleared;
}

bool is_line_full(const int* line, int width) {
  for (int col = 0; col < width; col++) {
    if (line[col] == 0) {
      return false;
    }
  }
  return true;
}

void shift_lines_down(int** matrix, int from_row) {
  for (int row = from_row - 1; row >= 0; row--) {
    for (int col = 0; col < GAME_FIELD_WIDTH; col++) {
      matrix[row + 1][col] = matrix[row][col];
    }
  }

  for (int col = 0; col < GAME_FIELD_WIDTH; col++) {
    matrix[0][col] = 0;
  }
}

void rotate_figure(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  if (CurrentState && CurrentBlock && !CurrentState->pause) {
    CurrentBlock->rotation = (CurrentBlock->rotation + 1) % 4;

    if (check_collision(CurrentState, CurrentBlock, false)) {
      CurrentBlock->rotation = (CurrentBlock->rotation - 1) % 4;
    }
  }
}