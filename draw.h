#include <ncurses.h>

#define OK_PAIR 1
#define WAR_PAIR 2
#define ERR_PAIR 3
#define CRIT_PAIR 4
#define NORM_PAIR 5
#define BOX_WIDTH 20

void draw_box(int x, int y, int h, int w);
void screen_update();
void* init_draw(void*);
