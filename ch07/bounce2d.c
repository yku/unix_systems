/*
 * bounce2d.c
 * 2Dimention版
 * 入力 s: 横方向のスピードダウン S: 縦方向のスピードダウン
 *      f: 横方向のスピードアップ F: 縦方向のスピードアップ
 *      Q: 終了
 * gcc bouce2d.c set_ticker.c -lcursers
 */
#include <string.h>
#include <curses.h>
#include <signal.h>
#include "bounce.h"

void set_up();
void ball_move(int);
void wrap_up();

struct ppball ball;

int main()
{
    int c;

    set_up();
    while( (c = getch()) != 'Q' )
    {
        if( c == 'f' ) ball.x_ttm--;
        else if( c == 's' ) ball.x_ttm++;
        else if( c == 'F' ) ball.y_ttm--;
        else if( c == 'S' ) ball.y_ttm++;
    }
    wrap_up();

    return 0;
}

void set_up()
{
   ball.y_pos = Y_INIT;
   ball.x_pos = X_INIT;
   ball.y_ttg = ball.y_ttm = Y_TTM;
   ball.x_ttg = ball.x_ttm = X_TTM;
   ball.y_dir = 1;
   ball.x_dir = 1;
   ball.symbol = DFL_SYMBOL;

   initscr();
   noecho();
   crmode();

   signal( SIGINT, SIG_IGN );
   mvaddch( ball.y_pos, ball.x_pos, ball.symbol );
   refresh();

   signal( SIGALRM, ball_move );
   set_ticker( 1000 / TICKS_PER_SEC );
}

void wrap_up()
{
    set_ticker( 0 );
    endwin();
}

void ball_move(int signum)
{
    int y_cur, x_cur, moved;

    signal( SIGALRM, SIG_IGN );
    
    y_cur = ball.y_pos;
    x_cur = ball.x_pos;

    if( ball.y_ttm > 0 && ball.y_ttg-- == 1 ){
        ball.y_pos += ball.y_dir;
        ball.y_ttg = ball.y_ttm;
        moved = 1;
    }

    if( ball.x_ttm > 0 && ball.x_ttg-- == 1 ){
        ball.x_pos += ball.x_dir;
        ball.x_ttg = ball.x_ttm;
        moved = 1;
    }

    if( moved ){
        mvaddch( y_cur, x_cur, BLANK );
        mvaddch( ball.y_pos, ball.x_pos, ball.symbol );
        bouce_or_lose( &ball );
        move( LINES-1, COLS-1 );
        refresh();
    }
    signal( SIGALRM, ball_move );
}

int bouce_or_lose(struct ppball *bp)
{
    int ret = 0;

    if( bp->y_pos == TOP_ROW ){
        bp->y_dir = 1;
        ret = 1;
    }
    else if( bp->y_pos == BOT_ROW ){
        bp->y_dir = -1;
        ret = 1;
    }

    if( bp->x_pos == LEFT_EDGE ){
        bp->x_dir = 1;
        ret = 1;
    }
    else if( bp->x_pos == RIGHT_EDGE ){
        bp->x_dir = -1;
        ret = 1;
    }

    return ret;
}
