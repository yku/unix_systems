/*
 * hello2.c
 * loop内でcursesを使うサンプル
 */
#include <stdio.h>
#include <curses.h>

int main()
{
    int i;

    initscr(); /* cursesライブラリとttyを初期化 */

    clear();
    for(i = 0; i < LINES; i++){
        move(i, i + 1);
        if( i & 0x1 )
            standout();
        addstr("Hello world");
        if( i & 0x1 )
            standend();
    }
    refresh();
    getch();

    endwin(); /* cursesをオフ */

    return 0;
}
