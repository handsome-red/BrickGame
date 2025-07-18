#ifndef COMMON_H
#define COMMON_H

#define GAME_FIELD_HEIGHT 20
#define GAME_FIELD_WIDTH 10
#define BLOCK_SIZE 4
#define DEFAULT_SPEED 500000

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

#endif
