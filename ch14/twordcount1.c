/*
 * twordcount1.c
 *
 * 2つのファイルの語数をカウント。
 * スレッド2個+カウンタ1個バージョン
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

int total_words;

int main(int argc, char** argv)
{
    pthread_t t1, t2;
    void *count_words(void *);

    if(argc != 3){
        printf("usage: %s file1 fil2\n", argv[0]);
        exit(1);
    }
    total_words = 0;
    pthread_create(&t1, NULL, count_words, (void *)argv[1]);
    pthread_create(&t2, NULL, count_words, (void *)argv[2]);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("%5d: total words\n", total_words);
}

void *count_words(void *f)
{
    char *filename = (char *)f;
    FILE *fp;
    int c, prevc = '\0';

    if( ( fp = fopen(filename, "r") ) != NULL ){
        while( ( c = getc(fp) ) != EOF ){
            if( !isalnum(c) && isalnum(prevc) )
                total_words++;
            prevc = c;
        }
        fclose(fp);
    }else
        perror(filename);
    return NULL;
}
