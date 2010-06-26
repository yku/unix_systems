/*
 * bounce1d.c
 * ハンドラを利用したアニメーション制御のサンプル
 */
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <signal.h>

#define MESSAGE "\\(^o^)/"
#define BLANK   "       "

int row;
int col;
int dir;

void move_msg(int);

int main()
{
    int delay; /* 値が大きいほど遅い */
    int new_delay;
    int c;

    initscr();
    crmode(); /* crmode(非カノニカルモード) */
    noecho();
    clear();

    row = 10; /* start point */
    col = 0;
    dir = 1;
    delay = 200;

    move(row, col);
    addstr(MESSAGE);
    signal(SIGALRM, move_msg);
    set_ticker( delay );

    while( 1 )
    {
        new_delay = 0;
        c = getch();
        if( c == 'Q' ) break;
        if( c == ' ' ) dir = -dir;
        if( c == 'f' && delay > 2 ) new_delay = delay / 2;
        if( c == 's' ) new_delay = delay * 2;
        if( new_delay > 0 ){
            delay = new_delay;
            set_ticker( delay );
        }
    }
    endwin();

    return 0;
}

void move_msg(int signum)
{
    signal(SIGALRM, move_msg);
    move( row, col );
    addstr( BLANK );
    col += dir;
    move( row, col );
    addstr( MESSAGE );
    refresh();

    if( dir == -1 && col <= 0 )
        dir = 1;
    else if( dir == 1 && col + strlen( MESSAGE ) >= COLS )
        dir = -1;
}

