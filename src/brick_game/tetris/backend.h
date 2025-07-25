/**
 * @file backend.h
 * @brief Header file for all backend functions
 */
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
/**
 * @brief Destroys a dynamically allocated 2D matrix
 *
 * Safely deallocates each row of the matrix and the matrix pointer itself.
 * Sets the matrix pointer to NULL after deallocation.
 *
 * @param[in,out] matrix_ptr Triple pointer to the matrix to destroy
 * @param[in] rows Number of rows in the matrix
 */
void destroy_matrix(int*** matrix_ptr, int rows);
/**
 * @brief Creates a new 2D matrix initialized with zeros
 *
 * Allocates memory for a rows x cols matrix using calloc.
 * Performs full error checking and cleans up if allocation fails.
 *
 * @param[in] rows Number of rows in the matrix
 * @param[in] cols Number of columns in the matrix
 * @return int** Pointer to the newly allocated matrix, or NULL if allocation
 * failed
 */
int** create_matrix(int rows, int cols);
/**
 * @brief Handles user input and translates it to game actions
 *
 * Processes all possible user actions and modifies game state accordingly.
 * Clears temporary figure before processing input to ensure clean state.
 *
 * @param[in] action The user action to process (from UserAction_t enum)
 * @param[in] hold Indicates if the action is being held (used for soft drop)
 *
 * Supported actions:
 * - Start: Resumes paused game
 * - Pause: Toggles pause state
 * - Terminate: Ends game and saves high score
 * - Left/Right: Moves block horizontally
 * - Up: Rotates block
 * - Down: Soft drop (when held)
 * - Action: Hard drop (instant fall)
 */
void userInput(UserAction_t action, bool hold);
/**
 * @brief Gets a copy of the current game state
 *
 * Retrieves the current game state through getCurrentState() and
 * returns a copy of it. Useful for safe state inspection.
 *
 * @return GameInfo_t Copy of the current game state
 */
GameInfo_t updateCurrentState();
/**
 * @brief Gets or initializes the current game state
 *
 * Manages the singleton game state instance. Initializes on first call with:
 * - Empty game field and next block matrices
 * - Zero score and loaded high score
 * - Initial level and preview pause state
 *
 * Also handles automatic game timing and block drawing when called.
 *
 * @return GameInfo_t* Pointer to current game state (singleton instance)
 *
 * @note Performs these automatic operations when called:
 * 1. Clears temporary figure markers
 * 2. Processes game timer ticks
 * 3. Updates game state via FiniteStateMachine()
 * 4. Redraws temporary figure
 */
GameInfo_t* getCurrentState();
UserAction_t readInput();
/**
 * @brief Gets the block state for a given tetromino type and rotation
 *
 * Returns the pre-defined coordinates for all 7 tetromino types
 * (I, J, L, O, S, T, Z) in their 4 possible rotation states.
 *
 * @param[in] BlockType The tetromino type (0-6)
 * @param[in] BlockState The rotation state (0-3)
 * @return TetrominoState Structure containing block coordinates
 */
TetrominoState blockState(int BlockType, int BlockState);
/**
 * @brief Transfers the next figure from preview to current block
 *
 * Detects the figure type from the next preview matrix and initializes
 * the current block with its properties. Sets default position and rotation.
 *
 * @param[in] CurrentState Pointer to the current game state
 * @param[out] block Pointer to the block structure to initialize
 */
void copy_next_to_block(GameInfo_t* CurrentState, GameBlock_t* block);
/**
 * @brief Controls the game timing based on level and pause state
 *
 * Calculates the appropriate interval between game updates based on level,
 * with minimum interval of 50ms. Returns 1 when the interval has passed
 * and game is not paused.
 *
 * @param[in] level Current game level (affects speed)
 * @param[in] pause Pause state (1 = paused, 0 = running)
 * @return int 1 if game should update, 0 otherwise
 */
int GameTimer(int level, int pause);
/**
 * @brief Main game state machine controller
 *
 * Manages transitions between all game states and delegates to appropriate
 * handler functions. Maintains internal state between calls.
 *
 * @param[in,out] CurrentState Pointer to current game state
 * @param[in,out] CurrentBlock Pointer to current active block
 * @return FSM The updated game state after processing
 *
 * State Transition Diagram:
 * - GAME_START → (via on_game_start) → SPAWN
 * - SPAWN → (via on_game_spawn) → MOVING
 * - MOVING → (via move_down) → MOVING or ATTACHING
 * - ATTACHING → (via on_attaching) → SPAWN or GAME_OVER
 * - GAME_OVER → (via on_game_over) → GAME_START
 *
 * @note Returns GAME_OVER immediately if null pointers are provided
 */
FSM FiniteStateMachine(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
/**
 * @brief Frees all memory allocated for the game state
 *
 * Safely deallocates the game field matrix, next figure matrix,
 * and the state structure itself. Sets the pointer to NULL.
 *
 * @param[in,out] CurrentState Double pointer to the game state to free
 */
void FreeCurrentState(GameInfo_t** CurrentState);
/**
 * @brief Fills a matrix with zeros
 *
 * @param[in,out] matrix The 2D matrix to be cleared
 * @param[in] row Number of rows in the matrix
 * @param[in] col Number of columns in the matrix
 */
void empty_matrix(int** matrix, int row, int col);
/**
 * @brief Prepares the next figure to be displayed in the "next" preview
 *
 * Generates a random tetromino type, calculates its position offset,
 * and stores it in the CurrentState->next matrix for display.
 *
 * @param[in,out] CurrentState Pointer to the current game state
 */
void prepare_next_figure(GameInfo_t* CurrentState);
/**
 * @brief Checks for collision between current block and game field
 *
 * Tests whether the current block would collide with field boundaries
 * or existing pieces. Can check either current position or predicted
 * position one cell below.
 *
 * @param[in] CurrentState Pointer to current game state
 * @param[in] CurrentBlock Pointer to current active block
 * @param[in] predict If true, checks position one cell below current
 * @return true if collision detected, false otherwise
 */
bool check_collision(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock,
                     bool predict);
/**
 * @brief Copies contents from one matrix to another
 *
 * Assumes both matrices are of size BLOCK_SIZE x BLOCK_SIZE.
 * Copies element by element from matrix_B to matrix_A.
 *
 * @param[out] matrix_A Destination matrix
 * @param[in] matrix_B Source matrix
 */
void copy_matrix(int** matrix_A, int** matrix_B);
/**
 * @brief Handles the figure attachment process and checks game over condition
 *
 * Permanently attaches the current figure to the field (marking with value 1)
 * and checks if there's space to spawn the next figure in the starting area.
 *
 * @param[in,out] state Pointer to the current game state
 * @param[in] block Pointer to the current game block
 * @return FSM Returns SPAWN if next figure can be spawned, GAME_OVER otherwise
 */
FSM foo_attaching(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
/**
 * @brief Moves block down one cell if possible
 *
 * Attempts to move block down, checking for collisions.
 * Only moves if game is not paused.
 *
 * @param[in,out] CurrentState Pointer to current game state
 * @param[in,out] CurrentBlock Pointer to current active block
 * @return FSM Next state (MOVING or ATTACHING)
 */
FSM move_down(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
/**
 * @brief Moves block left if possible
 *
 * Attempts to move block left one cell, checking for collisions.
 * Only moves if game is not paused. Reverts move if collision detected.
 *
 * @param[in,out] CurrentState Pointer to current game state
 * @param[in,out] CurrentBlock Pointer to current active block
 * @return FSM Always returns MOVING state
 */
FSM move_left(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
/**
 * @brief Moves block right if possible
 *
 * Attempts to move block right one cell, checking for collisions.
 * Only moves if game is not paused. Reverts move if collision detected.
 *
 * @param[in,out] CurrentState Pointer to current game state
 * @param[in,out] CurrentBlock Pointer to current active block
 * @return FSM Always returns MOVING state
 */
FSM move_right(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
/**
 * @brief Moves block down until it hits bottom or another block
 *
 * Repeatedly attempts to move the block down until collision is detected.
 * Uses move_down() internally and stops after maximum field height attempts.
 *
 * @param[in,out] CurrentState Pointer to current game state
 * @param[in,out] CurrentBlock Pointer to current active block
 * @return FSM Next state (MOVING or ATTACHING)
 */
FSM fall_down(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
/**
 * @brief Rotates the current game block if possible
 *
 * Checks for collisions after rotation and reverts if necessary.
 * Rotation only occurs when game is not paused.
 *
 * @param[in,out] CurrentState Pointer to the current game state
 * @param[in,out] CurrentBlock Pointer to the current game block
 */
void rotate_figure(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);

/**
 * @brief Clears all full lines in the game field and shifts lines down
 *
 * @param[in,out] matrix The game field matrix
 * @return int Number of lines that were cleared
 */
int clear_full_lines(int** matrix);

/**
 * @brief Checks if a line in the game field is completely filled
 *
 * @param[in] line Pointer to the line to check
 * @param[in] width Width of the game field (number of columns)
 * @return true if the line is completely filled
 * @return false if the line contains at least one empty cell
 */
bool is_line_full(const int* line, int width);
/**
 * @brief Shifts all lines above the specified row down by one
 *
 * @param[in,out] matrix The game field matrix
 * @param[in] from_row The row from which to start shifting down
 */
void shift_lines_down(int** matrix, int from_row);
/**
 * @brief Calculates score for cleared lines
 *
 * Implements standard Tetris scoring:
 * - 1 line: 100 points
 * - 2 lines: 300 points
 * - 3 lines: 700 points
 * - 4 lines: 1500 points
 *
 * @param[in] lines Number of lines cleared (1-4)
 * @return int Points awarded
 */
int count_score(int lines);
/**
 * @brief Calculates current level based on score
 *
 * Level increases every 600 points, capped at level 10.
 *
 * @param[in] score Current game score
 * @return int Current level (0-10)
 */
int lvl_up(int score);
/**
 * @brief Saves high score to file if it matches current record
 *
 * @param[in] score Score to save
 * @param[in] record Current high score to verify
 */
void save_record(int score, int record);
/**
 * @brief Updates high score record if new score is higher
 *
 * @param[in] score Current game score
 * @param[in] record Current high score
 * @return int New high score value
 */
int update_record(int score, int record);
/**
 * @brief Loads the high score from file
 *
 * Attempts to read the high score from "high_score.txt" file.
 * Returns 0 if file doesn't exist or can't be read.
 *
 * @return int The loaded high score, or 0 if not available
 */
int load_high_score();
/**
 * @brief Draws the current figure as temporary markers (value 2) on the field
 *
 * Calculates the figure's position based on its current state and marks
 * those positions on the field with value 2 (temporary). Only modifies
 * empty or already temporary cells.
 *
 * @param[in,out] state Pointer to the current game state
 * @param[in] block Pointer to the current game block
 */
void draw_temporary_figure(GameInfo_t* state, GameBlock_t* block);
/**
 * @brief Clears all temporary figure markers (value 2) from the game field
 *
 * Iterates through the entire game field and resets any cells marked
 * as temporary (value 2) back to empty (value 0).
 *
 * @param[in,out] state Pointer to the current game state
 */
void clear_temporary_figure(GameInfo_t* state);
/**
 * @brief Gets or resets the current game block
 *
 * Manages the singleton current block instance. Can be used to:
 * - Get the current block (reset = false)
 * - Reset and free the current block (reset = true)
 * Initializes new block with default position if none exists.
 *
 * @param[in] reset Whether to reset the current block
 * @return GameBlock_t* Pointer to current block, NULL if reset
 */
GameBlock_t* getCurrentBlock(bool reset);
/**
 * @brief Handles game over condition
 *
 * Saves high score if achieved and resets game to start state.
 *
 * @param[in,out] CurrentState Pointer to current game state
 * @return FSM Always returns GAME_START state
 */
FSM on_game_over(GameInfo_t* CurrentState);
/**
 * @brief Handles new block spawning
 *
 * Transfers the next block to current position and prepares a new next block.
 *
 * @param[in,out] CurrentState Pointer to current game state
 * @param[out] CurrentBlock Pointer to be initialized with new block
 * @return FSM Always returns MOVING state
 */
FSM on_game_spawn(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
/**
 * @brief Initializes a new game
 *
 * Resets game field, score, level, and prepares first block.
 *
 * @param[in,out] CurrentState Pointer to game state to initialize
 * @return FSM Always returns SPAWN state
 */
FSM on_game_start(GameInfo_t* CurrentState);
/**
 * @brief Handles the block attachment process
 *
 * Manages state transitions when a block needs to attach to the field.
 * Updates score, level, and high score if attachment occurs.
 *
 * @param[in,out] CurrentState Pointer to current game state
 * @param[in,out] CurrentBlock Pointer to current active block
 * @return FSM Next state (SPAWN or MOVING)
 */
FSM on_attaching(GameInfo_t* CurrentState, GameBlock_t* CurrentBlock);
/**
 * @brief Frees all dynamically allocated game resources
 *
 * Cleans up the game state matrix, next block matrix, and current block.
 * Safely handles NULL pointers and ensures complete memory deallocation.
 */
void free_resourse();
/**
 * @brief Resets the game state to initial conditions
 *
 * Clears both game field and next block matrices, resets score and level,
 * and prepares a new next figure. Does not affect high score.
 *
 * @param[in,out] state Pointer to game state to reset
 *
 * Reset Operations:
 * 1. Clears main game field matrix
 * 2. Clears next block matrix
 * 3. Resets score to 0
 * 4. Resets level to 0
 * 5. Updates speed to match level
 * 6. Generates new next figure
 */
void reset_game_state(GameInfo_t* state);

#endif