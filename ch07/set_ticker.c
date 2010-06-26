/*
 * set_ticker.c
 */
#include <sys/time.h>
/*
 * SIGALRMが周期的に発行されるようにインターバルタイマを設定する。
 * エラーなら-1、成功なら0を返す
 * 引数はミリ秒単位なので、秒とマイクロ秒の組み合わせに変換する。
 * set_ticker( 0 )とするとシグナルは送られない
 */
int set_ticker(int n_msecs )
{
    struct itimerval new_timeset;
    long n_sec, n_usecs;

    n_sec = n_msecs / 1000; /* 整数部 */
    n_usecs = ( n_msecs % 1000 ) * 1000L;

    /* 反復インターバルの設定 */
    new_timeset.it_interval.tv_sec = n_sec;
    new_timeset.it_interval.tv_usec = n_usecs;
    /* 初期インターバルの設定 */
    new_timeset.it_value.tv_sec = n_sec;
    new_timeset.it_value.tv_usec = n_usecs;
    
    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
