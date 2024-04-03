#include "main.h"
#include "gba.h"
#include "images/b29crash.h"
#include "images/b29fail.h"
#include "images/b29win.h"
#include "images/side_b29.h"
#include "states/end.h"
#include "states/play.h"
#include "states/start.h"

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Load initial application state
  enum gba_state state = DRAW_STARTSCREEN;
  struct game_data game = {.text_flash = 0,
                           .current_buttons = BUTTONS,
                           .title_text_pos = 0,
                           .previous_buttons = BUTTONS,
                           .state = DRAW_STARTSCREEN,
                           .altitude = HEIGHT - GROUND_HEIGHT - SIDE_B29_HEIGHT,
                           .distance = 0,
                           .x_vel = 0,
                           .y_vel = 0,
                           .prev_col = 0,
                           .prev_row = 0};

  while (1) {
    game.current_buttons = BUTTONS; // Load the current state of the buttons

    waitForVBlank();

    switch (state) {
    case DRAW_STARTSCREEN:
      state = draw_startscreen(&game);
      break;
    case WAIT_FOR_START:
      state = wait_for_start(&game);
      break;
    case DRAW_BG:
      state = draw_bg(&game);
      break;
    case PLAY:
      state = play(&game);
      break;
    case DRAW_LOSESCREEN:
      state = draw_losescreen(&game);
      break;
    case DRAW_FAILEDSCREEN:
      state = draw_failedscreen();
      break;
    case WAIT_FOR_RESTART:
      state = wait_for_restart(&game, WHITE, b29crash);
      break;
    case WAIT_FOR_RESTART_ON_WIN:
      state = wait_for_restart(&game, BLACK, b29win);
      break;
    case WAIT_FOR_RESTART_ON_FAIL:
      state = wait_for_restart(&game, RED, b29fail);
      break;
    case DRAW_WINSCREEN:
      state = draw_winscreen();
      break;
    }

    game.previous_buttons =
        game.current_buttons; // Store the current state of the buttons
    game.state = state;
  }

  return 0;
}
