#ifndef PLAY_H
#define PLAY_H

#include "../gba.h"
#include "../images/bomb.h"
#include "../images/down_b29.h"
#include "../images/explosion.h"
#include "../images/ground.h"
#include "../images/runway.h"
#include "../images/side_b29.h"
#include "../images/up_b29.h"
#include "../images/v2site.h"
#include "../main.h"

// Used for ground
#define DISP_DIST(d) ((d) / 10)

// I hate ints for physics...
#define GRAVITY 5
#define DRAG 1
#define MAX_SPEED 100

enum gba_state draw_bg(struct game_data *game);
enum gba_state play(struct game_data *game);
void draw_statusbar(struct game_data *game);
enum gba_state check_loss(struct game_data *game);
int clamp(int num, int min, int max);
void draw_ground(int distance);
void draw_explosion(int row, int col);
void draw_bomb(struct game_data *game);

#endif
