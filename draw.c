
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#include "log.h"
#include "glob.h"

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

struct time_with_unit{
	int time;
	char unit;
};

struct time_with_unit get_time_with_unit(int time_s) {
	if      (time_s > YEAR  *2) return (struct time_with_unit){.time = time_s/YEAR,   .unit='y'};
	else if (time_s > WEEK  *2) return (struct time_with_unit){.time = time_s/WEEK,   .unit='w'};
	else if (time_s > DAY   *2) return (struct time_with_unit){.time = time_s/DAY,    .unit='d'};
	else if (time_s > HOUR  *2) return (struct time_with_unit){.time = time_s/HOUR,   .unit='h'};
	else if (time_s > MINUTE*2) return (struct time_with_unit){.time = time_s/MINUTE, .unit='m'};
	else                        return (struct time_with_unit){.time = time_s,        .unit='s'};
}
void draw_host(struct host* host, enum token type, int x, int* y) {
  if(pthread_mutex_lock(&host->lock))CRITICAL_ERROR("Failed to lock mutex");

	int time_c = time(0);
	int last = time_c-host->last_seen;
	if(last<2)           attron(COLOR_PAIR(OK_PAIR));
	else if(last<5)      attron(COLOR_PAIR(WAR_PAIR));
	else if(type==HOST)  attron(COLOR_PAIR(ERR_PAIR));
	else if(type==CHOST) {attron(COLOR_PAIR(CRIT_PAIR)); fill_bg(x, *y, BOX_WIDTH, 3);}
	else if(type==OHOST) goto end;

	{
		draw_box(x, *y, BOX_WIDTH,  3); 
		if(last<5) {
			move(*y+1, x+2);
			printw("%s", host->name);
			move(*y+2, x+2);
			printw("RTT %.3fms", ((float)host->ping_us)/1000);
		}
		else {
			move(*y+1, x+2);
			printw("%s", host->name);
			move(*y+2, x+2);
			struct time_with_unit twu = get_time_with_unit(last);
			printw("last %d%c ago",twu.time, twu.unit);

		}
		*y+=4;
	}

end:
  if(pthread_mutex_unlock(&host->lock))CRITICAL_ERROR("Failed to unlock mutex");
}

void draw_newcol(int* x, int* y) {
	*x+=BOX_WIDTH+2; 
	*y = 3;
}
int ensure_space(int* x, int* y, int h) {
	if (*y+h >= LINES) {
		*x+=BOX_WIDTH+2; 
		*y = 3;
	}
	if (*x+BOX_WIDTH>=COLS) return 1;
	return 0;
}

int screen_update() {
	static int c = 0;
	char got = getch();
	static uint16_t disp = 1;
	if (got == 'q') return 1;
	if (got >= '0' && got <= '9')
		disp = 1<<(got-'0')>>1;
	erase();
	int x = 2;
	int y = 3;
	int is_fs = 1;
	for (int i = 0; i<vector_len(&drawcmd_vec); i++) {
	  struct drawcmd* cmd = vector_get(i, &drawcmd_vec);
	  if(!cmd)CRITICAL_ERROR("Failed to get drawcmd");
	  if((!(cmd->displays&disp)) && disp!=0) continue;

		switch(cmd->type) {
			case HOST:
			case OHOST:
			case CHOST:
				if(ensure_space(&x, &y, 4)) goto drf;
				draw_host((struct host*)cmd->data,cmd->type, x, &y); break;
			case BR:
				attron(COLOR_PAIR(NORM_PAIR));
				if(ensure_space(&x, &y, 1)) goto drf;
				draw_br(x, y, BOX_WIDTH);
				y+=1;
				break;
			case SECTION:
				if (!is_fs) draw_newcol(&x, &y);
				attron(COLOR_PAIR(NORM_PAIR));
				if(strlen((char*)cmd->data)+x<COLS)
					mvprintw(1,x+1,"%s", (char*)cmd->data);
				is_fs = 0;
				break;
			case LSECTION:
				attron(COLOR_PAIR(NORM_PAIR));
				if(ensure_space(&x, &y, 4)) goto drf;
				mvprintw(y+1,x+1,"%s", (char*)cmd->data);
				if(strlen((char*)cmd->data)+x<COLS)
					draw_br(x, y+2, BOX_WIDTH);
				y+=4;
				is_fs = 0;
				break;
			default:
  			CRITICAL_ERROR("Unexpected draw request");
		}
	}
drf:
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
