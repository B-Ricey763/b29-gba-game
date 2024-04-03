#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

#define GROUND_HEIGHT 10
#define TARGET_DIST 999
#define STATUSBAR_HEIGHT 20
#define PAD_SIZE 3

#define DIST_TO_TARGET(X) (TARGET_DIST - (X) / 100)

enum gba_state {
  DRAW_STARTSCREEN,
  WAIT_FOR_START,
  DRAW_BG,
  PLAY,
  DRAW_WINSCREEN,
  DRAW_LOSESCREEN,
  DRAW_FAILEDSCREEN,
  WAIT_FOR_RESTART,
  WAIT_FOR_RESTART_ON_WIN,
  WAIT_FOR_RESTART_ON_FAIL,
};

struct game_data {
  int text_flash;
  u32 current_buttons;
  u32 previous_buttons;
  enum gba_state state;
  int altitude;
  int distance;
  int x_vel;
  int y_vel;
  int prev_col;
  int prev_row;
  int bomb_prev_col;
  int bomb_prev_row;
  int bomb_alt;
  int bomb_x_vel;
  int bomb_y_vel;
  int bomb_dropped;
  int title_text_pos;
  int title_text_dir;
};
/*
 * For example, for a Snake game, one could be:
 *
 * struct snake {
 *   int heading;
 *   int length;
 *   int row;
 *   int col;
 * };
 *
 * Example of a struct to hold state machine data:
 *
 * struct state {
 *   int currentState;
 *   int nextState;
 * };
 *
 */

#endif
