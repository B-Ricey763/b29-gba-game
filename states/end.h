#ifndef END_H
#define END_H

#include "../images/b29crash.h"
#include "../images/b29fail.h"
#include "../images/b29win.h"
#include "../main.h"

enum gba_state draw_losescreen(struct game_data *game);
enum gba_state draw_winscreen(void);
enum gba_state draw_failedscreen(void);
enum gba_state wait_for_restart(struct game_data *game, u16 color,
                                const u16 *image);

#endif
