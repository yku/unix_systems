#include <stdio.h>
#include <signal.h>

void f(int);

int main()
{
    int i;

    signal( SIGINT, f );
    for(i = 0; i < 5; i++){
        printf("Hello work\n");
        sleep(1);
    }
}

void f(int signum)
{
    printf("OUCH!\n");
}
