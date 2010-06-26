/*
 * hello3.c
 * loop内でcursesを使うサンプル
 * sleepとrefreshを使ってアニメーション効果を与える
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
        sleep(1);
        refresh();
    }
    endwin(); /* cursesをオフ */

    return 0;
}
