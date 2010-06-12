/*
 * シグナルを無視するデモ
 * Ctrl-Cは効かない。Ctrl-\でquit
 */
#include <stdio.h>
#include <signal.h>

int main()
{
    signal( SIGINT, SIG_IGN );

    printf("You can not stop me!\n");
    while(1){
        sleep(1);
        printf("hahaha\n");
    }
    return 0;
}
