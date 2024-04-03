#include "end.h"
#include <stdio.h>

enum gba_state draw_winscreen(void) {
  drawFullScreenImageDMA(b29win);
  drawString(10, 10, "MISSION SUCCESS", BLACK);
  return WAIT_FOR_RESTART_ON_WIN;
}

enum gba_state draw_failedscreen(void) {
  drawFullScreenImageDMA(b29fail);
  drawString(10, 10, "MISSION FAILED", RED);
  return WAIT_FOR_RESTART_ON_FAIL;
}

enum gba_state draw_losescreen(struct game_data *game) {
  drawFullScreenImageDMA(b29crash);
  char num_buffer[100];
  snprintf(num_buffer, 100, "CRASHED %03d KM AWAY FROM TARGET",
           DIST_TO_TARGET(game->distance));

  drawString(10, 10, num_buffer, BLACK);
  return WAIT_FOR_RESTART;
}

enum gba_state wait_for_restart(struct game_data *game, u16 color,
                                const u16 *image) {
  if (KEY_JUST_PRESSED(BUTTON_SELECT, game->current_buttons,
                       game->previous_buttons)) {
    return DRAW_STARTSCREEN;
  } else if (game->text_flash == 1 && vBlankCounter % 30 == 0) {
    drawString(130, 100, "PRESS SELECT TO RESTART", color);
    game->text_flash = 0;
  } else if (game->text_flash == 0 && vBlankCounter % 30 == 0) {
    undrawImageDMA(130, 100, 138, 20, image);
    game->text_flash = 1;
  }
  return game->state;
}
