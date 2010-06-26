/*
 * hello1.c
 * cursesのサンプル
 */
#include <stdio.h>
#include <curses.h>

int main()
{
    initscr(); /* cursesライブラリとttyを初期化 */

    clear();
    move(10, 20);
    addstr("Hello world");
    move(LINES-1, 0); /* 左下隅にカーソルを移動 */

    refresh();
    getch();

    endwin(); /* cursesをオフ */

    return 0;
}
