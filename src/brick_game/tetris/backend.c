#include "./backend.h"

// void creating_shape() {}
FSM FiniteStateMachine(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  static FSM game_state = GAME_START;

  switch (game_state) {
    case GAME_START:
      empty_matrix(CurrentState->field, GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
      empty_matrix(CurrentState->next, BLOCK_SIZE, BLOCK_SIZE);
      CurrentState->level = 0;
      CurrentState->score = 0;
      prepare_next_figure(CurrentState);
      game_state = SPAWN;
      break;
    case MOVING:
      game_state = move_down(CurrentState, CurrentBlock);
      break;
    case SPAWN:
      copy_next_to_block(CurrentState, CurrentBlock);
      prepare_next_figure(CurrentState);
      game_state = MOVING;
      break;
    case ATTACHING:
      if (!Check_collision_V2(CurrentState, CurrentBlock)) {
        game_state = MOVING;
        break;
      }

      game_state = foo_attaching(CurrentState, CurrentBlock);
      CurrentState->score += count_score(full_line(CurrentState));
      // if (game_state == GAME_OVER) {
      //   exit(0);
      // }
      CurrentState->level = lvl_up(CurrentState->score);
      CurrentState->high_score =
          update_record(CurrentState->score, CurrentState->high_score);
      // game_state = SPAWN;
      break;
    case GAME_OVER:
      // exit(0);
      game_state = on_game_over(CurrentState);
      break;
    default:
      break;
  }
  return game_state;
}

FSM on_game_over(GameInfo_t* CurrentState) {
  if (CurrentState->score > CurrentState->high_score) {
    CurrentState->high_score = CurrentState->score;
    save_record(CurrentState->score, CurrentState->high_score);
  }
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

bool game_over(bool flag) {
  static bool game_over_flag = false;
  if (flag) {
    game_over_flag = true;
  } else {
    game_over_flag = false;
  }
  return game_over_flag;
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
  if (!CurrentState->pause) {
    if (!Check_collision_V2(CurrentState, CurrentBlock)) {
      CurrentBlock->x++;
    } else {
      state = ATTACHING;
    }
  }
  return state;
}

bool Check_collision_V2(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  TetrominoState coords =
      blockState(CurrentBlock->name, CurrentBlock->rotation);

  for (int i = 0; i < 4; i++) {
    int world_x = CurrentBlock->x + coords.blocks[i].x + 1;
    int world_y = CurrentBlock->y + coords.blocks[i].y;

    if (world_x >= GAME_FIELD_HEIGHT || world_y < 0 ||
        world_y >= GAME_FIELD_WIDTH ||
        (world_x >= 0 && CurrentState->field[world_x][world_y] > 0)) {
      return true;
    }
  }
  return false;
}

// реализовать функцию которая будет отдельно делать проверки для каждого
// состояни ЩШЫВАЖЫВОЛПТЖОДЫВПЖОЫВРПЖДОЫРВПЖЛОЫРВЖАДОЫВДй
FSM fall_down(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  FSM state = move_down(CurrentState, CurrentBlock);  // добавить define
  while (state == MOVING) {
    state = move_down(CurrentState, CurrentBlock);
  }

  //  CurrentBlock->x++;
  return state;
}

FSM move_left(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  FSM state = MOVING;
  if (!CurrentState->pause) {
    CurrentBlock->y--;
    if (Check_collision(CurrentState, CurrentBlock)) {
      CurrentBlock->y++;
      // state = ATTACHING;
    }
  }

  return state;
}

FSM move_right(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  FSM state = MOVING;
  if (!CurrentState->pause) {
    CurrentBlock->y++;
    if (Check_collision(CurrentState, CurrentBlock)) {
      CurrentBlock->y--;
      // state = ATTACHING;
    }
  }

  return state;
}

FSM foo_detachment(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  TetrominoState coords =
      blockState(CurrentBlock->name, CurrentBlock->rotation);

  for (int i = 0; i < 4; i++) {
    int x = CurrentBlock->x + coords.blocks[i].x;
    int y = CurrentBlock->y + coords.blocks[i].y;

    // if ((x >= 0 && x < GAME_FIELD_HEIGHT && y >= 0 && y < GAME_FIELD_WIDTH)
    // &&
    //     CurrentState->field[x][y] > 1) {
    //   CurrentState->field[x][y] = 0;
    // }
    if ((x >= 0 && x < GAME_FIELD_HEIGHT && y >= 0 && y < GAME_FIELD_WIDTH)) {
      CurrentState->field[x][y] = 0;
    }
  }
  return MOVING;
}

bool Check_collision(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  TetrominoState coords =
      blockState(CurrentBlock->name, CurrentBlock->rotation);

  for (int i = 0; i < 4; i++) {
    int world_x = CurrentBlock->x + coords.blocks[i].x;
    int world_y = CurrentBlock->y + coords.blocks[i].y;

    if (world_x >= GAME_FIELD_HEIGHT || world_y < 0 ||
        world_y >= GAME_FIELD_WIDTH ||
        (world_x >= 0 && CurrentState->field[world_x][world_y] > 0)) {
      return true;
    }
  }
  return false;
}

/*
Добавить флаг который будет запрещать перещать фигуру при attaching
*/
void userInput(UserAction_t action, bool hold) {
  (void)hold;  // ATENTION

  GameInfo_t* CurrentState = getCurrentState();
  GameBlock_t* CurrentBlock = getCurrentBlock();
  // FSM state = -1;

  if (CurrentState) {
    clear_temporary_figure(CurrentState);
  }

  switch (action) {
    case Start:
      CurrentState->pause = !CurrentState->pause;
      break;
    case Pause:
      CurrentState->pause = !CurrentState->pause;
      break;
    case Terminate:
      save_record(CurrentState->score, CurrentState->high_score);
      FreeCurrentState(&CurrentState);
      if (CurrentBlock != NULL) {
        free(CurrentBlock);
        CurrentBlock = NULL;
      }
      CurrentState = NULL;
      game_over(true);
      break;
    case Left:
      move_left(CurrentState, CurrentBlock);
      break;
    case Right:
      move_right(CurrentState, CurrentBlock);
      break;
    case Up:
      roll_figure(CurrentState, CurrentBlock);
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

  int interval =
      1000 /
      (1 +
       level * 0.5);  // JKHADFLKJHDAFKJHADFKJHDKFJHKLJHLKJDHFLKJAHDFKJHALKDJFH
  // int interval = 1000 / (1 + level * 10);
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
  GameInfo_t result = {0};
  if (CurrentState) {
    result = *CurrentState;
  }
  return result;
}

GameBlock_t* getCurrentBlock() {
  static GameBlock_t* CurrentBlock = NULL;
  if (NULL == CurrentBlock) {
    CurrentBlock = (GameBlock_t*)malloc(sizeof(GameBlock_t));
    CurrentBlock->name = -1;
    CurrentBlock->rotation = 0;
    CurrentBlock->x = 0;
    CurrentBlock->y = GAME_FIELD_WIDTH / 2 - 2;
    for (int i = 0; i < 4; i++) {  // Использовать функцию матриц
      CurrentBlock->coords.blocks[i].x = 0;
      CurrentBlock->coords.blocks[i].y = 0;
    }
  }

  return CurrentBlock;
  // return (CurrentBlock && CurrentBlock->name != -1) ? CurrentBlock : NULL;
}

/**
 * Добавить флаг для того чтобы контролировать
 */
GameInfo_t* getCurrentState() {
  static GameInfo_t* CurrentState = NULL;  // Убрать указатель
  GameBlock_t* CurrentBlock = getCurrentBlock();
  if (NULL == CurrentState) {
    CurrentState = (GameInfo_t*)malloc(sizeof(GameInfo_t));

    if (NULL != CurrentState) {
      CurrentState->field = create_matrix(GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
      CurrentState->next = create_matrix(BLOCK_SIZE, BLOCK_SIZE);

      if (CurrentState->field == NULL || CurrentState->next == NULL) {
        FreeCurrentState(&CurrentState);
      } else {
        CurrentState->score = 0;
        FILE* file = fopen("./high_score.txt", "r");  // Отдельная функция
        if (file) {
          fscanf(file, "%d", &CurrentState->high_score);
          fclose(file);
        } else {
          CurrentState->high_score = 0;
        }

        CurrentState->level = 0;
        CurrentState->speed = DEFAULT_SPEED;
        CurrentState->pause = 0;
      }
    }
  } else {
    FSM state = -1;                        // define
    clear_temporary_figure(CurrentState);  // открепить фигуру от поля

    if (GameTimer(CurrentState->level, CurrentState->pause)) {
      state = FiniteStateMachine(CurrentState, CurrentBlock);
    }
    draw_temporary_figure(CurrentState,
                          CurrentBlock);  // прикрепить фигуру к полю
    if (state == ATTACHING) clear_temporary_figure(CurrentState);
  }
  return CurrentState;
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
  // FSM game_over = SPAWN;
  bool can_spawn = true;

  for (int y = 0; y < GAME_FIELD_WIDTH; y++) {
    for (int x = 0; x < 3; x++) {  // Проверяем 3 верхних ряда
      if (state->field[x][y] != 0) {
        can_spawn = false;
        break;
      }
    }
    if (!can_spawn) break;
  }

  // Прикрепляем фигуру к полю (значение 1)
  for (int i = 0; i < 4; i++) {
    int x = block->x + coords.blocks[i].x;
    int y = block->y + coords.blocks[i].y;

    if (x >= 0 && x < GAME_FIELD_HEIGHT && y >= 0 && y < GAME_FIELD_WIDTH) {
      state->field[x][y] = 1;  // Окончательное прикрепление
    }
  }

  return can_spawn ? SPAWN : GAME_OVER;
}

void copy_next_to_block(GameInfo_t* CurrentState, GameBlock_t* block) {
  // Проверка входных параметров
  if (!CurrentState || !CurrentState->next || !block) {
    return;
  }

  // Определяем тип фигуры в next (по ненулевым значениям)
  int figure_type = -1;
  for (int i = 0; i < BLOCK_SIZE && figure_type == -1; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      if (CurrentState->next[i][j] != 0) {
        figure_type = CurrentState->next[i][j] - 1;
        break;
      }
    }
  }

  // Если фигура не найдена
  if (figure_type < 0 || figure_type > Z) {
    return;
  }

  // Устанавливаем параметры блока
  block->name = (TetrominoName)figure_type;
  block->rotation = 0;                  // Начальное состояние поворота
  block->x = -1;                        // Стартовая позиция по вертикали
  block->y = GAME_FIELD_WIDTH / 2 - 1;  // Центрирование по горизонтали

  // Копируем координаты фигуры из предопределённых состояний
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
    int col = next.blocks[i].x + offset_x;  // столбец = x + смещение
    int row = next.blocks[i].y + offset_y;  // строка = y + смещение

    if (row >= 0 && row < BLOCK_SIZE && col >= 0 && col < BLOCK_SIZE) {
      CurrentState->next[col][row] = next_type + 1;
    }
  }
}

void FreeCurrentState(GameInfo_t** CurrentState) {
  if (*CurrentState) {
    destroy_matrix((*CurrentState)->field, GAME_FIELD_HEIGHT);
    destroy_matrix((*CurrentState)->next, BLOCK_SIZE);
    free(*CurrentState);
    *CurrentState = NULL;  // Важно!
  }
}

void destroy_matrix(int** matrix, int size) {
  for (int i = 0; i < size; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

int** create_matrix(int rows, int cols) {
  int** matrix = (int**)calloc(rows, sizeof(int*));
  for (int i = 0; i < rows; i++) {
    matrix[i] = (int*)calloc(cols, sizeof(int));
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

int full_line(GameInfo_t* CurrentState) {
  int counter = 0;
  for (int i = GAME_FIELD_HEIGHT - 1; i >= 0; i--) {
    bool full = true;  // Заполнена
    for (int j = 0; j < GAME_FIELD_WIDTH && full; j++) {
      if (CurrentState->field[i][j] == 0) {
        full = false;  // Линия не заполнена
      }
    }

    if (full) {
      for (int m = i - 1; m >= 0; m--) {
        for (int k = 0; k < GAME_FIELD_WIDTH; k++) {
          CurrentState->field[m + 1][k] = CurrentState->field[m][k];
          if (m == 0) {
            CurrentState->field[m][k] = 0;
          }
        }
      }
      i++;
      counter++;
    }
  }

  return counter;
}

void roll_figure(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock) {
  if (!CurrentState->pause) {
    CurrentBlock->rotation = (CurrentBlock->rotation + 1) % 4;

    if (Check_collision(CurrentState, CurrentBlock)) {
      CurrentBlock->rotation = (CurrentBlock->rotation - 1) % 4;
    }
  }
}

// void roll_figure(int pause, GameBlock_t* CurrentBlock) {
//   if (!CurrentState->pause) {
//     CurrentBlock->rotation = (CurrentBlock->rotation + 1) % 4;

//     if (Check_collision(CurrentState, CurrentBlock)) {
//       CurrentBlock->rotation = (CurrentBlock->rotation - 1) % 4;
//     }
//   }
// }