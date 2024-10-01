#include <ncurses.h>
#include "setup.h"
#include <time.h>
#include "log.h"

#define OK_PAIR 1
#define WAR_PAIR 2
#define ERR_PAIR 3
#define CRIT_PAIR 4
#define NORM_PAIR 5
#define BOX_WIDTH 20

void draw_box(int x, int y, int w, int h) {
	mvhline(y, x+1, 0, w-1);
	mvhline(y+h, x+1, 0, w-1);
	mvvline(y+1, x, 0, h-1);
	mvvline(y+1, x+w, 0, h-1);
	mvaddch(y, x, ACS_ULCORNER); 
	mvaddch(y, x+w, ACS_URCORNER); 
	mvaddch(y+h, x, ACS_LLCORNER); 
	mvaddch(y+h, x+w, ACS_LRCORNER);
}

void draw_br(int x, int y, int w) { 
	mvhline(y, x+1, 0, w-1);
}

void fill_bg(int x, int y, int w , int h) {
	for (int xi = x; xi<x+w; xi++)
		for (int yi = y; yi<y+h; yi++)
			mvaddch(yi, xi, ' ');
}

#define MINUTE 60
#define HOUR 60*MINUTE
#define DAY 24*HOUR
#define WEEK 7*DAY
#define YEAR 365*DAY

void draw_device(char* name, int id, int x, int* y, int type) {
	int time_c = time(0);
	int last = time_c-hosts[id].last_seen;
	if(last<2) attron(COLOR_PAIR(OK_PAIR));
	else if(last<5) attron(COLOR_PAIR(WAR_PAIR));
	else if(type==0) attron(COLOR_PAIR(ERR_PAIR));
	else if(type==1) {attron(COLOR_PAIR(CRIT_PAIR)); fill_bg(x, *y, BOX_WIDTH, 3);}
	else if (type==2) return;

	draw_box(x, *y, BOX_WIDTH,  3); 
	if(last<5) {
		move(*y+1, x+2);
		printw("%s", name);
		move(*y+2, x+2);
		printw("RTT %.3fms", ((float)hosts[id].ping_us)/1000);
	}
	else {
		move(*y+1, x+2);
		printw("%s", name);
		move(*y+2, x+2);
		char unit = 's'; int last_ad = last;
		if (last > YEAR*2) { last_ad = last/YEAR; unit='y'; }
		else if (last > WEEK*2) { last_ad = last/WEEK; unit='w'; }
		else if (last > DAY*2) { last_ad = last/DAY; unit='d'; }
		else if (last > HOUR*2) { last_ad = last/HOUR; unit='h'; }
		else if (last > MINUTE*2) { last_ad = last/MINUTE; unit='m'; }
		printw("last %d%c ago",last_ad, unit);

	}
	*y+=4;
	
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
				draw_device(drawc[i].name, devc++, x, &y, 0); break;
			case DRAW_CHOST:
				draw_device(drawc[i].name, devc++, x, &y, 1); break;
			case DRAW_OHOST:
				draw_device(drawc[i].name, devc++, x, &y, 2); break;
			case DRAW_BR:
				attron(COLOR_PAIR(NORM_PAIR));
				draw_br(x, y, BOX_WIDTH);
				y+=1;
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
			case DRAW_LSECTION:
				attron(COLOR_PAIR(NORM_PAIR));
				mvprintw(y+1,x+1,"%s", drawc[i].name);
				draw_br(x, y+2, BOX_WIDTH);
				y+=4;
				is_fs = 0;
				break;
		}
	}
	{
		attron(COLOR_PAIR(NORM_PAIR));
		time_t sec = time(NULL);
		struct tm* tm_info = localtime(&sec);
		char time_buf[32];
		strftime(time_buf, 32, "%Y-%m-%d %H:%M:%S", tm_info);
		mvprintw(0,COLS-BOX_WIDTH,"%s", time_buf);
	}
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
