/*
 * bounce_async.c
 * 
 * fdにO_ASYNCを指定して、非同期入出力を実現するサンプル
 * whileループのポーリングを省エネ化
 */
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <signal.h>
#include <fcntl.h>

#define MESSAGE "\\(^o^)/"
#define BLANK   "       "

int row = 10;
int col = 0;
int dir = 1;
int delay = 200;
int done = 0;

void on_input(int);
void on_alarm(int);
void enable_kbd_signals();

int main()
{
    initscr();
    crmode(); /* crmode(非カノニカルモード) */
    noecho();
    clear();

    signal( SIGIO, on_input );
    enable_kbd_signals(); /* キーボードシグナルをON */
    signal( SIGALRM, on_alarm );
    set_ticker( delay );

    move( row, col );
    addstr( MESSAGE );
    
    while( !done )
        pause();
    
    endwin();
    
    return 0;
}

void on_input(int signum)
{
    int c = getch();
    if( c == 'Q' || c == EOF )
        done = 1;
    else if( c == ' ' )
        dir = -dir;
}

void on_alarm(int signum)
{
    signal( SIGALRM, on_alarm ); /* リセット。念のため */
    mvaddstr( row, col, BLANK );
    col += dir;
    mvaddstr( row, col, MESSAGE );
    refresh();

    if( dir == -1 && col <= 0 )
        dir = 1;
    else if( dir == 1 && col + strlen( MESSAGE ) >= COLS )
        dir = -1;
}

void enable_kbd_signals()
{
    int fd_flags;

    fcntl( 0, F_SETOWN, getpid() );
    fd_flags = fcntl( 0, F_GETFL );
    fcntl( 0, F_SETFL, (fd_flags | O_ASYNC) );
}

