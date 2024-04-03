#include "play.h"
#include <stdio.h>

/*
  Clamp a number between min and max
*/
int clamp(int num, int min, int max) {
  if (num < min) {
    return min;
  } else if (num > max) {
    return max;
  } else {
    return num;
  }
}

/*
  Initial background draw, also resets state.
  This is not strictly necessary since DRAW_STARTSCREEN also resets it, but in
  case you ever go straight from the restart screen to this, its good to go
*/
enum gba_state draw_bg(struct game_data *game) {
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

  // draw initial screen
  drawRectDMA(0, 0, WIDTH, HEIGHT - GROUND_HEIGHT, SKY_BLUE);
  drawRectDMA(0, 0, WIDTH, STATUSBAR_HEIGHT, GRAY);
  drawString(0, 0, "DIST:", WHITE);
  draw_ground(game->distance);
  return PLAY;
}

/*
  Contains the main game logic (and calls to the functions that affect it)
  The physics is jank, but hey, it works
*/
enum gba_state play(struct game_data *game) {
  // save these locally to (hopefully) save on perf
  int x_vel = game->x_vel;
  int y_vel = game->y_vel;

  if (KEY_JUST_PRESSED(BUTTON_SELECT, game->current_buttons,
                       game->previous_buttons)) {
    return DRAW_STARTSCREEN;
  }

  // GETTING THROTTLE
  int delta_throttle = 0;
  if (KEY_DOWN(BUTTON_UP, game->current_buttons)) {
    delta_throttle += 1;
  }

  // GETTING PITCH & CHANGING IMAGE
  int delta_pitch = 0;
  const unsigned short *b29_to_draw;
  // These can be constant since all have the same size
  int b29_width = SIDE_B29_WIDTH;
  int b29_height = SIDE_B29_HEIGHT;
  if (KEY_DOWN(BUTTON_LEFT, game->current_buttons)) {
    b29_to_draw = up_b29;
    delta_pitch += 1;
  } else if (KEY_DOWN(BUTTON_RIGHT, game->current_buttons)) {
    b29_to_draw = down_b29;
    delta_pitch -= 1;
  } else {
    b29_to_draw = side_b29;
  }

  // calcualte x_vel based on a bunch of factors
  // when you pitch up, you will slow down, when down, speed up
  x_vel = clamp(x_vel + delta_throttle * 2 - delta_pitch - DRAG, 0, MAX_SPEED);
  // Gives you a value from [0, 4]
  int x_vel_contrib = x_vel / (MAX_SPEED / 4);
  // gives you a pitch from -3 to 3
  int pitch_contrib = delta_pitch * 3;
  // This calc gives you decent game feel given the situation
  y_vel = GRAVITY - x_vel_contrib - pitch_contrib;

  // Save everything back to data
  game->x_vel = x_vel;
  game->altitude = clamp(game->altitude + y_vel, STATUSBAR_HEIGHT,
                         HEIGHT - GROUND_HEIGHT - b29_height);
  game->distance += x_vel;

  // Visual only
  int col = clamp((x_vel - MAX_SPEED / 50), 70, 170);
  // shouldn't use this much DMA, but it works!
  drawRectDMA(game->prev_row, game->prev_col, b29_width, b29_height, SKY_BLUE);
  // actually draw the plane
  drawImageDMA(game->altitude, col, b29_width, b29_height, b29_to_draw);
  game->prev_row = game->altitude;
  game->prev_col = col;

  // BOMB HANDLING
  if (KEY_JUST_PRESSED(BUTTON_B, game->current_buttons,
                       game->previous_buttons) &&
      game->bomb_dropped == 0) {
    game->bomb_dropped = 1;
    game->bomb_alt = game->altitude + SIDE_B29_HEIGHT;
    game->bomb_y_vel = x_vel;
    game->bomb_prev_col = col;
    game->bomb_prev_row = game->bomb_alt;
    game->y_vel = y_vel;
  }

  // Draw the other stuff
  draw_statusbar(game);
  draw_ground(game->distance);
  draw_bomb(game);
  return check_loss(game);
}

/*
  Despite the name, checks all sorts of win/loss conditions.
*/
enum gba_state check_loss(struct game_data *game) {
  int target_dist = DIST_TO_TARGET(game->distance);
  int bomb_cutoff = HEIGHT - GROUND_HEIGHT - V2SITE_HEIGHT / 2;
  // check for different win/lose conditions
  if (game->altitude >= HEIGHT - GROUND_HEIGHT - SIDE_B29_HEIGHT &&
      game->distance > WIDTH * 20) {
    // you spent too much time on the ground before leaving the runway, so loss
    return DRAW_LOSESCREEN;
  } else if (game->bomb_dropped == 1 && game->bomb_alt >= bomb_cutoff &&
             game->bomb_prev_col > target_dist &&
             game->bomb_prev_col < target_dist + V2SITE_WIDTH) {
    // BOMB was successfully dropped
    draw_explosion(game->bomb_prev_row, game->bomb_prev_col);
    return DRAW_WINSCREEN;
  } else if ((game->bomb_dropped == 1 && game->bomb_alt >= bomb_cutoff) ||
             (target_dist < -WIDTH)) {
    // Bomb was dropped, but didn't hit the target
    draw_explosion(game->bomb_prev_row, game->bomb_prev_col);
    return DRAW_FAILEDSCREEN;
  }
  return PLAY;
}

void draw_explosion(int row, int col) {
  drawImageDMA(row, col, EXPLOSION_WIDTH, EXPLOSION_HEIGHT, explosion);
}

void draw_statusbar(struct game_data *game) {
  if (vBlankCounter % 15 == 0) {

    int offset = 36; // for text;
    drawRectDMA(0, offset, PAD_SIZE * 6, STATUSBAR_HEIGHT, GRAY);
    char buffer[PAD_SIZE + 1];
    snprintf(buffer, PAD_SIZE + 1, "%0*d", PAD_SIZE,
             DIST_TO_TARGET(game->distance));
    drawString(0, offset, buffer, WHITE);
  }
}

void draw_ground(int raw_dist) {
  int distance = DISP_DIST(raw_dist);
  int row = HEIGHT - GROUND_HEIGHT;
  if (distance < WIDTH) {
    // The runway moves back relative to screen as player 'moves' forward
    drawPartialLeftImage(row, 0, distance, RUNWAY_WIDTH, RUNWAY_HEIGHT, runway);
    // as that happens, the normal ground creeps in to make the transition
    // seamless
    int offset = RUNWAY_WIDTH - distance;
    drawPartialRightImage(row, offset - 1, GROUND_WIDTH, GROUND_HEIGHT, ground);
    // return early for perf
    return;
  }
  // this is some gnarly math to get the site to draw, but by god it works!
  // the idea is three stages: first it is off the screen to the right, then its
  // on the screen, and then its off the screen to the left, each using their
  // respective function
  int target_dist = DIST_TO_TARGET(raw_dist);
  int v2Row = HEIGHT - GROUND_HEIGHT - V2SITE_HEIGHT;
  if (target_dist < WIDTH && target_dist > WIDTH - V2SITE_WIDTH) {
    drawPartialRightImage(v2Row, target_dist, V2SITE_WIDTH, V2SITE_HEIGHT,
                          v2site);
  } else if (target_dist > 0 && target_dist <= WIDTH - V2SITE_WIDTH) {
    drawImageDMA(v2Row, target_dist, V2SITE_WIDTH, V2SITE_HEIGHT, v2site);
  } else if (target_dist <= 0 && target_dist > -V2SITE_WIDTH) {
    drawPartialLeftImage(v2Row, 0, -target_dist, V2SITE_WIDTH, V2SITE_HEIGHT,
                         v2site);
  }

  // Once the the runway is gone, we can just loop the image normally based off
  // distance to give the illusion of traveling. Since dist is based off speed,
  // the ground movement varies based off how fast you go.
  drawLoopingImage(distance, GROUND_WIDTH, GROUND_HEIGHT, ground);
}

/*
  Despite the name (again), this handles both bomb drawing and the handling of
  the velocity in x and y
*/
void draw_bomb(struct game_data *game) {
  if (game->bomb_dropped) {
    drawRectDMA(game->bomb_prev_row, game->bomb_prev_col, BOMB_WIDTH,
                BOMB_HEIGHT, SKY_BLUE);
    game->bomb_y_vel = GRAVITY - 3;
    game->bomb_alt += game->bomb_y_vel;
    drawImageDMA(game->bomb_alt, game->bomb_prev_col, BOMB_WIDTH, BOMB_HEIGHT,
                 bomb);
    game->bomb_prev_row = game->bomb_alt;
    if (vBlankCounter % 30 == 0) {
      game->bomb_prev_col -= 1;
    }
  }
}
