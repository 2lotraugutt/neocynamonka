#include <ncurses.h>

#define OK_PAIR 1
#define WAR_PAIR 2
#define ERR_PAIR 3
#define CRIT_PAIR 4
#define NORM_PAIR 5
#define BOX_WIDTH 20

void draw_box(int x, int y, int h, int w) {
	for(int yi = y+1; yi < y+w; yi++) { mvaddch(yi, x, '|'); mvaddch(yi, x+h, '|'); }
	for(int xi = x+1; xi < x+h; xi++) { mvaddch(y, xi, '-'); mvaddch(y+w, xi, '-'); } 
	mvaddch(y, x, '+'); mvaddch(y+w, x, '+'); mvaddch(y, x+h, '+'); mvaddch(y+w, x+h, '+');
}

int screen_update() {
	static int c = 0;
	char got = getch();
	if (got == 'q') return 1;
	clear();
	move(1, 2+2);
	attron(COLOR_PAIR(NORM_PAIR));
	printw("Serverownia");
	int y = 3;
	draw_box(2,  y, BOX_WIDTH,  10);
	draw_box(2,  y+=10, BOX_WIDTH,  10);
	move(1, BOX_WIDTH+6);
	printw("Blok A");
	draw_box(BOX_WIDTH+4, 3 , BOX_WIDTH, 2);
	move(1, BOX_WIDTH*2+8);
	printw("Blok B");
	draw_box(BOX_WIDTH*2+6, 3 , BOX_WIDTH, 2);
	refresh();
	return 0;
}

void* init_draw(void* null) {
	initscr();
	keypad(stdscr, TRUE);
	timeout(100);
	curs_set(0);
	cbreak();
	noecho();
	start_color();
	init_pair(NORM_PAIR, COLOR_WHITE, COLOR_BLACK);
	init_pair(OK_PAIR, COLOR_GREEN, COLOR_BLACK);
	init_pair(WAR_PAIR, COLOR_YELLOW, COLOR_BLACK);
	init_pair(ERR_PAIR, COLOR_RED, COLOR_BLACK);
	init_pair(CRIT_PAIR, COLOR_WHITE, COLOR_RED);
	while(!screen_update());
	endwin();

	return 0;
}
