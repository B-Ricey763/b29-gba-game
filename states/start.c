#include "start.h"
#include "../images/side_b29.h"

enum gba_state draw_startscreen(struct game_data *game) {
  // reset state
  game->title_text_pos = -10;
  game->distance = 0;
  game->altitude = HEIGHT - SIDE_B29_HEIGHT - GROUND_HEIGHT;
  game->x_vel = 0;
  game->y_vel = 0;
  game->prev_col = 0;
  game->prev_row = STATUSBAR_HEIGHT;
  game->bomb_prev_col = 0;
  game->bomb_prev_row = STATUSBAR_HEIGHT;
  game->bomb_x_vel = 0;
  game->bomb_y_vel = 0;
  game->bomb_alt = 0;
  game->bomb_dropped = 0;
  game->title_text_dir = 1;

  drawFullScreenImageDMA(B29Fullscreen);
  drawString(10, 10, "B29 SUPERFORTRESS", WHITE);
  return WAIT_FOR_START;
}

enum gba_state wait_for_start(struct game_data *game) {
  if (KEY_JUST_PRESSED(BUTTON_START, game->current_buttons,
                       game->previous_buttons)) {
    return DRAW_BG;
  } else if (game->text_flash == 1 && vBlankCounter % 30 == 0) {
    undrawImageDMA(130, 100, 114, 20, B29Fullscreen);
    drawString(120, 100, "PRESS START TO PLAY", WHITE);
    game->text_flash = 0;
  } else if (game->text_flash == 0 && vBlankCounter % 30 == 0) {
    undrawImageDMA(120, 100, 114, 20, B29Fullscreen);
    drawString(130, 100, "PRESS START TO PLAY", WHITE);
    game->text_flash = 1;
  }
  return WAIT_FOR_START;
}
