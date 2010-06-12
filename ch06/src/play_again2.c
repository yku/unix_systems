/*
 * play_again2.c
 * ユーザーが他のトランザクションを望んでいるかたずねる
 * 非カノニカルモードを利用することで即座に応答を返すように変更する。
 * エコーモードをOFFにして、無効なキーを無視する。
 * y なら 0 を返す
 * n なら 1 を返す
 */
#include <stdio.h>
#include <termios.h>

#define QUESTION "Do you want another transaction"

int get_response(char *);
void set_crmode(void); 
int tty_mode(int);

int main()
{
    int response;

    tty_mode(0);
    set_crmode();
    response = get_response( QUESTION );
    tty_mode(1);
    return response;
}

/*
 * y/n以外の回答を無視する。yなら0,nなら1を返す。
 */
int get_response(char* str)
{
    printf("%s (y/n) ?", str);
    while(1){
        switch( getchar() ){
            case 'y':
            case 'Y': return 0;
            case 'n':
            case 'N': 
            case EOF: return 1;
        }
    }
}

/*
 * file descripter 0 をcrモードにする。
 */
void set_crmode()
{
    struct termios ttystate;

    tcgetattr( 0, &ttystate ); /* file descripter 0 の状態をttystateに書き出す */
    ttystate.c_lflag &= ~ICANON; /* buffering off */
    ttystate.c_lflag &= ~ECHO; /* echo off */
    ttystate.c_cc[VMIN] = 1; /* 一度に一文字ずつ取得する */
    tcsetattr( 0, TCSANOW, &ttystate ); /* set new state */
}

/*
 * モードの保存および復元を行う。
 * 0 => save, 1 => load
 */
int tty_mode(int how)
{
    static struct termios mode;
    if( how == 0 )
        return tcgetattr( 0, &mode );
    else
        return tcsetattr( 0, TCSANOW, &mode );
}
