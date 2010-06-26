/*
 * sleep1.c
 *
 * sleepの仕組みを示す。SIGALRMハンドラ、alarm()、pause()
 */
#include <stdio.h>
#include <signal.h>

void wakeup(int);

int main()
{
    int sleeptime = 4;

    printf("about to sleep for %d seconds\n", sleeptime);
    signal( SIGALRM, wakeup );
    alarm( sleeptime );
    pause();
    printf("Morning so soon?\n");

    return 0;
}

void wakeup(int signum)
{
    printf("Alarm received from kernel\n");
}
