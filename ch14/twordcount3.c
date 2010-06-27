/*
 * twordcount3.c
 *
 * 2つのファイルの語数をカウント。
 * スレッド2個+カウンタ2個+スレッドに複数の引数（構造体利用）
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

struct arg_set {
    char *fname;
    int count;
};


int main(int argc, char** argv)
{
    pthread_t t1, t2;
    struct arg_set args1, args2;
    void *count_words(void *);

    if(argc != 3){
        printf("usage: %s file1 fil2\n", argv[0]);
        exit(1);
    }

    args1.fname = argv[1];
    args1.count = 0;
    pthread_create(&t1, NULL, count_words, (void *) &args1);

    args2.fname = argv[2];
    args2.count = 0;
    pthread_create(&t2, NULL, count_words, (void *) &args2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("%5d: %s\n", args1.count, argv[1]);
    printf("%5d: %s\n", args2.count, argv[2]);
    printf("%5d: total words\n", args1.count + args2.count);
}

void *count_words(void *a)
{
    struct arg_set *args = a;
    FILE *fp;
    int c, prevc = '\0';

    if( ( fp = fopen(args->fname, "r") ) != NULL ){
        while( ( c = getc(fp) ) != EOF ){
            if( !isalnum(c) && isalnum(prevc) )
                args->count++;
            prevc = c;
        }
        fclose(fp);
    }else
        perror(args->fname);
    return NULL;
}
