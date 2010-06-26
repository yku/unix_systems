/*
 * bounce_aio.c
 * 
 * gcc bounce_aio.c set_ticker.c -lrt -lcurses
 */
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <signal.h>
#include <aio.h>

#define MESSAGE "\\(^o^)/"
#define BLANK   "       "

int row = 10;
int col = 0;
int dir = 1;
int delay = 200;
int done = 0;

struct aiocb kbcbuf;

void on_input(int);
void on_alarm(int);
void setup_aio_buffer();

int main()
{
    initscr();
    crmode(); /* crmode(非カノニカルモード) */
    noecho();
    clear();

    signal( SIGIO, on_input );
    setup_aio_buffer();
    aio_read( &kbcbuf );

    signal( SIGALRM, on_alarm );
    set_ticker( delay );

    mvaddstr( row, col, MESSAGE );

    while( !done )
        pause();
    
    endwin();
    
    return 0;
}

void on_input(int signum)
{
    int c;
    char *cp = (char *) kbcbuf.aio_buf;
    
    if( aio_error( &kbcbuf ) != 0 )
        perror("reading failed");
    else
        if( ( aio_return( &kbcbuf ) ) == 1 )
        {
            c = *cp;
            if( c == 'Q' || c == EOF )
                done = 1;
            else if( c == ' ' )
                dir = -dir;
        }

    aio_read( &kbcbuf );
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

void setup_aio_buffer()
{
    static char input[1];

    kbcbuf.aio_fildes = 0; /* file descriptor */
    kbcbuf.aio_buf = input; /* buffer */
    kbcbuf.aio_nbytes = 1; /* 読み出す文字数 */
    kbcbuf.aio_offset = 0; /* ファイルのオフセット */

    /* 読み出しが完了したときになにをすべきか */
    kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    kbcbuf.aio_sigevent.sigev_signo = SIGIO; /* SIGIOを送る */
}
