/* bounce.h */
#ifndef _BOUNCE_H_
#define _BOUNCE_H_

#define OWATA "\\(^o^)/"
#define DFL_SYMBOL 'o'
//#define BLANK "       " dont use string via mvaddch
#define BLANK ' '
#define TOP_ROW 5
#define BOT_ROW 20
#define LEFT_EDGE 10
#define RIGHT_EDGE 70
#define X_INIT 10
#define Y_INIT 10
#define TICKS_PER_SEC 50

#define X_TTM 5 /* ticks to move */
#define Y_TTM 8 /* ticks to move */

struct ppball {
    int x_pos, y_pos;
    int x_ttm, y_ttm;
    int x_ttg, y_ttg; 
    int x_dir, y_dir;
    char symbol;
};

#endif

