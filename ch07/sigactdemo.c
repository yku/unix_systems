/*
 * sigactdemo.c
 *
 * sigaction()の使い方サンプル
 * C-cの処理中は、C-\をブロックする。
 * C-cのハンドラを設定し直さないので二度目で終了
 */
#include <stdio.h>
#include <signal.h>

#define INPUTLEN 100

void inthandler();

int main()
{
    struct sigaction newhandler;
    sigset_t blocked;
    char x[INPUTLEN];

    newhandler.sa_handler = inthandler;
    newhandler.sa_flags = SA_RESETHAND | SA_RESTART;

    /* blockするシグナルのリストを作る */
    sigemptyset( &blocked );
    sigaddset( &blocked, SIGQUIT );

    newhandler.sa_mask = blocked; /* block mask を組み込む */

    if( sigaction( SIGINT, &newhandler, NULL ) == -1 )
        perror("sigaction");
    else
        while( 1 ){
            fgets( x, INPUTLEN, stdin );
            printf("input: %s", x);
        }
}

void inthandler(int s)
{
    printf("Called with signal %d\n", s);
    sleep( s );
    printf("done handling signal %d\n", s);
}

