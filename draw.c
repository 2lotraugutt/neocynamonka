#include <ncurses.h>
#include "hosts.h"
#include "hostnr.h"
#include <time.h>

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

void draw_br(int x, int y, int h) { 
	for(int xi = x; xi <= x+h; xi++) { mvaddch(y, xi, '-');} 
}


void draw_device(char* name, int id, int x, int* y) {
	int time_c = time(0);
	if(time_c-hosts[id].last_seen<2) attron(COLOR_PAIR(OK_PAIR));
	else if(time_c-hosts[id].last_seen<5) attron(COLOR_PAIR(WAR_PAIR));
	else attron(COLOR_PAIR(ERR_PAIR));

	draw_box(x, *y, BOX_WIDTH,  5); 
	if(time_c-hosts[id].last_seen<5) {
		move(*y+2, x+2);
		printw("%s", name);
		move(*y+3, x+2);
		printw("RTT %.3fms", ((float)hosts[id].ping_us)/1000);
	}
	else {
		move(*y+2, x+2);
		printw("%s", name);
		move(*y+3, x+2);
		printw("last %ds ago",time_c-hosts[id].last_seen);

	}
	*y+=6;
	
}

int screen_update() {
	static int c = 0;
	char got = getch();
	if (got == 'q') return 1;
	erase();
	int x =2;
	int y = 3;
	int devc= 0;
	int is_fs = 1;
	for (int i = 0; i<drawcc; i++) {
		switch( drawc[i].type ) {
			case DRAW_HOST:
				draw_device(drawc[i].name, devc++, x, &y); break;
			case DRAW_BR:
				attron(COLOR_PAIR(NORM_PAIR));
				draw_br(x, y+1, BOX_WIDTH);
				y+=3;
				break;
			case DRAW_SECTION:
				if (!is_fs) {
					x+=BOX_WIDTH+2; 
					y = 3;
				}
				attron(COLOR_PAIR(NORM_PAIR));
				mvprintw(1,x+1,"%s", drawc[i].name);
				is_fs = 0;
				break;
		}
	}
	/* draw_device("Router", HOST_ROUTER, x, &y); */
	/* draw_device("ServSW 1", HOST_SWITCH_SERV1, x, &y); */
	/* draw_device("ServSW 2", HOST_SWITCH_SERV2, x, &y); */
	/* draw_device("ServSW 3", HOST_SWITCH_SERV3, x, &y); */
	refresh();
	return 0;
}

void* init_draw(void* null) {
	initscr();
	keypad(stdscr, TRUE);
	timeout(500);
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
