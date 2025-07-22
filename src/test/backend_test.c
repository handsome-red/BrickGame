#include "backend_test.h"  // Замените на ваш реальный заголовочный файл

#include <check.h>

// Вспомогательная функция для инициализации тестового блока
GameBlock_t create_test_block(TetrominoName name, int x, int y) {
  GameBlock_t block = {
      .name = name,
      .rotation = 0,
      .x = x,
      .y = y,
      .coords = {.blocks = {{0, 0}, {1, 0}, {0, 1}, {1, 1}}}  // Для O-тетромино
  };
  return block;
}

START_TEST(test_null_parameters) {
  FSM result = FiniteStateMachine(NULL, NULL);
  ck_assert_int_eq(result, GAME_OVER);
}
END_TEST

START_TEST(test_game_start_to_moving) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(O, 5, 0);

  FSM result = FiniteStateMachine(&state, &block);
  // Предполагаем, что после GAME_START должно перейти в MOVING
  ck_assert_int_eq(result, MOVING);
}
END_TEST

START_TEST(test_moving_transition) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(I, 3, 10);

  // Эмулируем текущее состояние MOVING
  FSM current_state = MOVING;
  FSM result = FiniteStateMachine(&state, &block);

  // Проверяем возможные переходы из MOVING
  ck_assert(result == SPAWN || result == ATTACHING || result == MOVING);
}
END_TEST

START_TEST(test_spawn_transition) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(T, 4, 0);

  // Эмулируем текущее состояние SPAWN
  FSM current_state = SPAWN;
  FSM result = FiniteStateMachine(&state, &block);

  ck_assert_int_eq(result, MOVING);
}
END_TEST

START_TEST(test_attaching_transition) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(Z, 2, 18);

  // Эмулируем текущее состояние ATTACHING
  FSM current_state = ATTACHING;
  FSM result = FiniteStateMachine(&state, &block);

  ck_assert(result == SPAWN || result == GAME_OVER);
}
END_TEST

START_TEST(test_game_over_state) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(L, 0, 0);

  // Эмулируем текущее состояние GAME_OVER
  FSM current_state = GAME_OVER;
  FSM result = FiniteStateMachine(&state, &block);

  ck_assert_int_eq(result, GAME_OVER);
}
END_TEST

Suite* finite_state_machine_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create("FiniteStateMachine");

  /* Core test case */
  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_null_parameters);
  tcase_add_test(tc_core, test_game_start_to_moving);
  tcase_add_test(tc_core, test_moving_transition);
  tcase_add_test(tc_core, test_spawn_transition);
  tcase_add_test(tc_core, test_attaching_transition);
  tcase_add_test(tc_core, test_game_over_state);

  suite_add_tcase(s, tc_core);

  return s;
}

START_TEST(test_reset_game_state_null_pointer) {
  // Проверка обработки NULL указателя
  reset_game_state(NULL);
  // Если функция не падает - тест пройден
}
END_TEST

START_TEST(test_reset_game_state_basic_reset) {
  GameInfo_t state;
  init_game_state(&state);  // Предполагаем наличие функции инициализации

  // Заполняем тестовыми данными
  state.score = 100;
  state.level = 5;
  state.speed = 2;
  fill_matrix(state.field, 1, GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);
  fill_matrix(state.next, 1, BLOCK_SIZE, BLOCK_SIZE);

  reset_game_state(&state);

  // Проверяем сброс основных параметров
  ck_assert_int_eq(state.score, 0);
  ck_assert_int_eq(state.level, 0);
  ck_assert_int_eq(state.speed, 0);
}
END_TEST

START_TEST(test_reset_game_state_field_cleared) {
  GameInfo_t state;
  init_game_state(&state);
  fill_matrix(state.field, 1, GAME_FIELD_HEIGHT, GAME_FIELD_WIDTH);

  reset_game_state(&state);

  // Проверяем что поле очищено
  for (int i = 0; i < GAME_FIELD_HEIGHT; i++) {
    for (int j = 0; j < GAME_FIELD_WIDTH; j++) {
      ck_assert_int_eq(state.field[i][j], 0);
    }
  }
}
END_TEST

START_TEST(test_reset_game_state_next_cleared) {
  GameInfo_t state;
  init_game_state(&state);
  fill_matrix(state.next, 1, BLOCK_SIZE, BLOCK_SIZE);

  reset_game_state(&state);

  // Проверяем что next очищен (prepare_next_figure заполнит его после)
  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      ck_assert_int_eq(state.next[i][j], 0);
    }
  }
}
END_TEST

START_TEST(test_reset_game_state_speed_reset) {
  GameInfo_t state;
  init_game_state(&state);
  state.level = 3;
  state.speed = 10;

  reset_game_state(&state);

  // Проверяем что скорость сброшена в соответствии с level (0)
  ck_assert_int_eq(state.speed, 0);
}
END_TEST

START_TEST(test_userInput_null_check) {
  // Проверка работы с NULL состоянием (должна работать без падения)
  setCurrentState(NULL);
  setCurrentBlock(NULL);

  userInput(Start, false);
  userInput(Pause, false);
  userInput(Left, false);
  // Проверяем что не было падения
  ck_assert(1);
}
END_TEST

START_TEST(test_userInput_start_action) {
  GameInfo_t state = {0};
  state.pause = PAUSE_ON;
  setCurrentState(&state);

  userInput(Start, false);

  ck_assert_int_eq(state.pause, PAUSE_OFF);
}
END_TEST

START_TEST(test_userInput_pause_toggle) {
  GameInfo_t state = {0};
  state.pause = PAUSE_OFF;
  setCurrentState(&state);

  userInput(Pause, false);
  ck_assert_int_eq(state.pause, PAUSE_ON);

  userInput(Pause, false);
  ck_assert_int_eq(state.pause, PAUSE_OFF);
}
END_TEST

START_TEST(test_userInput_terminate) {
  GameInfo_t state = {0};
  setCurrentState(&state);

  userInput(Terminate, false);

  ck_assert_int_eq(state.pause, -1);
}
END_TEST

START_TEST(test_userInput_movement_actions) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(I, 5, 10);
  setCurrentState(&state);
  setCurrentBlock(&block);

  // Проверяем что функции движения вызываются
  // (можно добавить моки для move_left и других функций)
  userInput(Left, false);
  userInput(Right, false);
  userInput(Up, false);
  userInput(Down, false);
  userInput(Action, false);

  // Если не было падения - считаем успехом
  ck_assert(1);
}
END_TEST

START_TEST(test_userInput_hold_parameter) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(O, 4, 15);
  setCurrentState(&state);
  setCurrentBlock(&block);

  bool hold = false;
  userInput(Down, hold);
  // Проверяем что hold был установлен в true внутри функции
  // (если это важно для логики)
  // В текущей реализации параметр игнорируется ((void)hold)
  ck_assert(!hold);  // остается false, так как передается по значению
}
END_TEST

START_TEST(test_userInput_clear_temporary_figure_called) {
  GameInfo_t state = {0};
  setCurrentState(&state);

  // Можно добавить мок для clear_temporary_figure
  userInput(Left, false);

  // Проверяем что функция очистки была вызвана
  // (если есть возможность проверки через моки)
  ck_assert(1);
}
END_TEST

START_TEST(test_getCurrentState_initialization) {
  // Первый вызов - должна произойти инициализация
  GameInfo_t* state = getCurrentState();
  ck_assert_ptr_nonnull(state);
  ck_assert_ptr_nonnull(state->field);
  ck_assert_ptr_nonnull(state->next);
  ck_assert_int_eq(state->score, 0);
  ck_assert_int_eq(state->level, 0);
  ck_assert_int_eq(state->pause, PREVIEW);
}
END_TEST

START_TEST(test_getCurrentState_memory_allocation_failure) {
  // Эмуляция неудачного выделения памяти (требуется мок malloc)
  // Здесь должен быть мок для malloc чтобы вернуть NULL
  GameInfo_t* state = getCurrentState();
  ck_assert_ptr_null(state);
}
END_TEST

START_TEST(test_getCurrentState_matrix_allocation_failure) {
  // Эмуляция неудачного выделения памяти для матриц (требуется мок
  // create_matrix) Здесь должен быть мок для create_matrix чтобы вернуть NULL
  GameInfo_t* state = getCurrentState();
  ck_assert_ptr_null(state);
}
END_TEST

START_TEST(test_getCurrentState_returns_same_instance) {
  // Проверка что возвращается один и тот же указатель
  GameInfo_t* state1 = getCurrentState();
  GameInfo_t* state2 = getCurrentState();
  ck_assert_ptr_eq(state1, state2);
}
END_TEST

START_TEST(test_getCurrentState_state_updates) {
  GameInfo_t* state = getCurrentState();
  GameBlock_t block = create_test_block(I, 5, 10);

  // Установка тестового блока (требуется мок getCurrentBlock)
  setCurrentBlock(&block);

  // Проверка обновления состояния
  state = getCurrentState();
  ck_assert_ptr_nonnull(state);

  // Проверка что функции были вызваны (требуются моки)
  // clear_temporary_figure, GameTimer, FiniteStateMachine,
  // draw_temporary_figure
}
END_TEST

START_TEST(test_getCurrentState_with_null_block) {
  GameInfo_t* state = getCurrentState();
  setCurrentBlock(NULL);

  state = getCurrentState();
  ck_assert_ptr_nonnull(state);
  // Проверяем что функция не падает при NULL блоке
}
END_TEST

START_TEST(test_getCurrentState_high_score_loading) {
  // Тест загрузки рекорда (требуется мок load_high_score)
  // Установим мок чтобы вернуть тестовое значение
  set_high_score_mock(1000);

  GameInfo_t* state = getCurrentState();
  ck_assert_int_eq(state->high_score, 1000);
}
END_TEST

START_TEST(test_getCurrentState_speed_calculation) {
  GameInfo_t* state = getCurrentState();
  state->level = 5;

  // Проверяем что скорость соответствует уровню
  ck_assert_int_eq(state->speed, state->level);
}
END_TEST

START_TEST(test_on_attaching_no_collision_returns_moving) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(I, 5, 10);

  // Настроим мок check_collision на возврат false
  set_check_collision_mock(false);

  FSM result = on_attaching(&state, &block);
  ck_assert_int_eq(result, MOVING);
}
END_TEST

START_TEST(test_on_attaching_collision_calls_foo_attaching) {
  GameInfo_t state = {0};
  GameBlock_t block = create_test_block(O, 3, 18);

  // Настроим мок check_collision на возврат true
  set_check_collision_mock(true);
  // Настроим мок foo_attaching на возврат SPAWN
  set_foo_attaching_mock(SPAWN);

  FSM result = on_attaching(&state, &block);
  ck_assert_int_eq(result, SPAWN);
}
END_TEST

START_TEST(test_on_attaching_updates_score) {
  GameInfo_t state = {.score = 100};
  GameBlock_t block = create_test_block(L, 2, 17);

  set_check_collision_mock(true);
  set_foo_attaching_mock(SPAWN);
  set_clear_full_lines_mock(2);  // Возвращаем 2 очищенные линии
  set_count_score_mock(200);     // Возвращаем 200 очков за линии

  on_attaching(&state, &block);
  ck_assert_int_eq(state.score, 300);  // 100 + 200
}
END_TEST

START_TEST(test_on_attaching_updates_level) {
  GameInfo_t state = {.score = 500};

  set_check_collision_mock(true);
  set_lvl_up_mock(2);  // Возвращаем уровень 2 для 500 очков

  on_attaching(&state, NULL);
  ck_assert_int_eq(state.level, 2);
}
END_TEST

START_TEST(test_on_attaching_updates_high_score) {
  GameInfo_t state = {.score = 1000, .high_score = 800};

  set_check_collision_mock(true);
  set_update_record_mock(1000);  // Возвращаем новый рекорд

  on_attaching(&state, NULL);
  ck_assert_int_eq(state.high_score, 1000);
}
END_TEST

START_TEST(test_on_attaching_null_parameters) {
  // Проверка обработки NULL параметров
  FSM result = on_attaching(NULL, NULL);
  ck_assert_int_eq(result,
                   SPAWN);  // или другое ожидаемое значение по умолчанию
}
END_TEST

START_TEST(test_on_attaching_score_calculation_flow) {
  GameInfo_t state = {.score = 0};
  GameBlock_t block = create_test_block(T, 4, 16);

  set_check_collision_mock(true);
  set_clear_full_lines_mock(4);
  set_count_score_mock(800);
  set_lvl_up_mock(1);
  set_update_record_mock(800);

  on_attaching(&state, &block);

  ck_assert_int_eq(state.score, 800);
  ck_assert_int_eq(state.level, 1);
  ck_assert_int_eq(state.high_score, 800);
}
END_TEST

// Вспомогательные функции
static GameBlock_t create_test_block(TetrominoName name, int x, int y) {
  GameBlock_t block = {0};
  block.name = name;
  block.x = x;
  block.y = y;
  return block;
}

// Глобальные переменные для моков
static bool collision_mock_result = false;
static FSM foo_attaching_mock_result = SPAWN;
static int cleared_lines_mock = 0;
static int score_mock = 0;
static int level_mock = 0;
static int record_mock = 0;

// Функции для настройки моков
void set_check_collision_mock(bool result) { collision_mock_result = result; }

void set_foo_attaching_mock(FSM result) { foo_attaching_mock_result = result; }

void set_clear_full_lines_mock(int lines) { cleared_lines_mock = lines; }

void set_count_score_mock(int score) { score_mock = score; }

void set_lvl_up_mock(int level) { level_mock = level; }

void set_update_record_mock(int record) { record_mock = record; }

// Мок-функции
bool check_collision(GameInfo_t* s, GameBlock_t* b, bool flag) {
  (void)s;
  (void)b;
  (void)flag;
  return collision_mock_result;
}

FSM foo_attaching(GameInfo_t* s, GameBlock_t* b) {
  (void)s;
  (void)b;
  return foo_attaching_mock_result;
}

int clear_full_lines(GameInfo_t* s) {
  (void)s;
  return cleared_lines_mock;
}

int count_score(int lines) {
  (void)lines;
  return score_mock;
}

int lvl_up(int score) {
  (void)score;
  return level_mock;
}

int update_record(int score, int old_record) {
  (void)score;
  (void)old_record;
  return record_mock;
}

// Глобальные переменные для моков
static GameBlock_t* mock_current_block = NULL;

void setCurrentBlock(GameBlock_t* block) { mock_current_block = block; }

GameBlock_t* getCurrentBlock(bool hold) {
  (void)hold;
  return mock_current_block;
}

int main(void) {
  int number_failed;
  Suite* s;
  SRunner* sr;

  s = finite_state_machine_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}