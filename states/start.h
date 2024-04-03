#ifndef START_H
#define START_H

#include "../images/b29fs.h"
#include "../main.h"

enum gba_state draw_startscreen(struct game_data *game);
enum gba_state wait_for_start(struct game_data *game);

#endif
