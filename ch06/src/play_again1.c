/*
 * play_again1.c
 * ユーザーが他のトランザクションを望んでいるかたずねる
 * 非カノニカルモードを利用することで即座に応答を返すように変更する。
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

int get_response(char* str)
{
    int input;
    printf("%s (y/n) ?", str);
    while(1){
        switch( input = getchar() ){
            case 'y':
            case 'Y': return 0;
            case 'n':
            case 'N': 
            case EOF: return 1;
            default:
                      printf("\ncannot understand %c, ", input);
                      printf("Please type y or no \n");
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
