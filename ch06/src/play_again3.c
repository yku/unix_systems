/*
 * play_again3.c
 * ユーザーが他のトランザクションを望んでいるかたずねる
 * 非カノニカルモードを利用することで即座に応答を返すように変更する。
 * エコーモードをOFFにして、無効なキーを無視する。
 * 遅延なしモードにして文字を読み込み結果を返す。
 * y なら 0 を返す
 * n なら 1 を返す
 */
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#define QUESTION "Do you want another transaction"
#define TRIES 3 
#define SLEEPTIME 2
#define BEEP putchar('\a');

int get_response(char *, int);
void set_cr_noecho_mode(void); 
void set_nodelay_mode(void);
int tty_mode(int);
int get_ok_char();

int main()
{
    int response;

    tty_mode(0);
    set_cr_noecho_mode();
    set_nodelay_mode();
    response = get_response( QUESTION, TRIES );
    tty_mode(1);
    return response;
}

/*
 * y/n以外の回答を無視する。yなら0,nなら1を返す。
 */
int get_response(char* str, int tries)
{
    int input;
    printf("%s (y/n) ?", str);
    fflush(stdout);
    while(1){
        sleep( SLEEPTIME );
        input = tolower( get_ok_char() );
        if( input == 'y' ) return 0;
        if( input == 'n' ) return 1;
        if( tries-- == 0 )  return 2;
        BEEP;
    }
}

/*
 * 認められていない文字を読み飛ばしy, n, Y, N, EOF のいずれかを返す。
 */
int get_ok_char()
{
    int c;
    while( ( c = getchar() ) != EOF && strchr( "yYnN", c ) == NULL )
        ;
    return c;
}

/*
 * file descripter 0 をcrモードにする。
 */
void set_cr_noecho_mode()
{
    struct termios ttystate;

    tcgetattr( 0, &ttystate ); /* file descripter 0 の状態をttystateに書き出す */
    ttystate.c_lflag &= ~ICANON; /* buffering off */
    ttystate.c_lflag &= ~ECHO; /* echo off */
    ttystate.c_cc[VMIN] = 1; /* 一度に一文字ずつ取得する */
    tcsetattr( 0, TCSANOW, &ttystate ); /* set new state */
}

/*
 * file descripter 0 を遅延なしモードにする
 */
void set_nodelay_mode()
{
    int flags;

    flags = fcntl( 0, F_GETFL ); /* 現在の設定を読み出す */
    flags |= O_NDELAY; /* 遅延なしビットをONにする。 */
    fcntl( 0, F_SETFL, flags );
}

/*
 * モードの保存および復元を行う。
 * 0 => save, 1 => load
 */
int tty_mode(int how)
{
    static struct termios mode;
    static int flags;
    if( how == 0 ){
        tcgetattr( 0, &mode );
        flags = fcntl( 0, F_GETFL ); 
    }else{
        tcsetattr( 0, TCSANOW, &mode );
        fcntl( 0, F_SETFL, flags );
    }
}
