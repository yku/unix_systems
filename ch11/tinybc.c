/**
 * bc をつかった仕事をする電卓
 * 双方向パイプの使い方を示す。
 * 概要
 * 1. 2本のパイプをつくる
 * 2. fork
 * 3. dc起動させるプロセスの中で標準入出力をパイプに接続し、execl dc
 * 4. pipeを介してやりとり
 * 5. close pipe
 *
 * stdin  >0 ==> ------ ==> to_dc   ==>  ----
 *              |tinybc|                | dc |  
 * stdout <1 <== ------ <== from_dc <==  ----
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define oops(m, x) { perror(m); exit(x); }

void be_dc(int in[2], int out[2]);
void be_bc(int in[2], int out[2]);
void fatal(char* message);

int main()
{
    int pid;
    int to_dc[2], from_dc[2];

    if ( pipe(to_dc) == -1 || pipe(from_dc) == -1 )
        oops("pipe failed", 1);
    if ( (pid = fork()) == -1)
        oops("cannot fork", 2);
    if ( pid == 0 ) { /* child */
        be_dc(to_dc, from_dc);
    } else {
        be_bc(to_dc, from_dc);
        wait(NULL); /* parent wait for child */
    }

    return 0;
}

/**
 * dcはstdinを受付けstdoutに出力するため、
 * dup2でコピーしている
 */
void be_dc(int in[2], int out[2])
{
    /* pipeの読み出し先をfd0にコピー */
    if ( dup2(in[0], 0) == -1 )
        oops("dc: cannot redirect stdin", 3);
    close(in[0]);
    close(in[1]);
    
    /* pipeの書き出し先をfd1にコピー */
    if ( dup2(out[1], 1) == -1 )
        oops("dc: cannot redirect stdout", 4);
    close(out[1]);
    close(out[0]);

    execlp("dc", "dc", "-", NULL);
    oops("Cannot run dc", 5);
}

void be_bc(int to_dc[2], int from_dc[2])
{
    int num1, num2;
    char operation[BUFSIZ], message[BUFSIZ];
    FILE *fpout, *fpin;

    close(to_dc[0]); /* 読み込み側をclose */
    close(from_dc[1]); /* 書き込み側をclose */

    fpout = fdopen( to_dc[1], "w" );
    fpin = fdopen( from_dc[0], "r" );
    if ( fpout == NULL || fpin == NULL )
        fatal("Error convering pipes to streams");

    while( fgets(message, BUFSIZ, stdin) != NULL ){
        printf("tinybc: ");
        if ( sscanf(message, "%d%[-+*/^]%d", &num1, operation, &num2) != 3) {
            printf("syntax error\n");
            continue;
        }

        if ( fprintf(fpout, "%d\n%d\n%c\np\n", num1, num2, *operation) == EOF ) 
            fatal("Error writing");
        fflush( fpout );
        if ( fgets(message, BUFSIZ, fpin) == NULL ) // dcから出力がかえってくるまでblock（待っている）。
            break;
        printf("%d %c %d = %s", num1, *operation, num2, message);
    }
    fclose(fpout);
    fclose(fpin);
}

void fatal(char *message)
{
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}
