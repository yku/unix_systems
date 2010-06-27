/*
 * twordcount4.c
 *
 * 条件変数を使って速報を届ける
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

struct arg_set {
    char *fname;
    char *tname; /* thread name */
    int count;
};

struct arg_set *mailbox = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t flag = PTHREAD_COND_INITIALIZER;

int main(int argc, char** argv)
{
    pthread_t t1, t2;
    struct arg_set args1, args2;
    void *count_words(void *);
    int reports = 0;
    int total_words = 0;

    if(argc != 3){
        printf("usage: %s file1 fil2\n", argv[0]);
        exit(1);
    }
    pthread_mutex_lock(&lock);

    args1.fname = argv[1];
    args1.tname = "A";
    args1.count = 0;
    pthread_create(&t1, NULL, count_words, (void *) &args1);

    args2.fname = argv[2];
    args2.tname = "B";
    args2.count = 0;
    pthread_create(&t2, NULL, count_words, (void *) &args2);

    while( reports < 2 ){
        printf("MAIN   : waiting for flag to go up\n");
        pthread_cond_wait(&flag, &lock); /* lockをアトミックに解放し、flagがシグナリングされるのを待つ */
        printf("MAIN   : wow! flag was raised, I have the lock\n");
        printf("%7d: %s\n", mailbox->count, mailbox->fname);
        total_words += mailbox->count;
        if( mailbox == &args1 )
            pthread_join(t1, NULL);
        if( mailbox == &args2 )
            pthread_join(t2, NULL);
        mailbox = NULL;
        pthread_cond_signal(&flag);
        reports++;
    }
    printf("%7d: total words\n", total_words);
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

    printf("COUNT_%s: waiting to get lock\n", args->tname);
    pthread_mutex_lock(&lock);
    printf("COUNT_%s: have lock, storing data\n", args->tname);
    if( mailbox != NULL ){
        printf("COUNT_%s: oops..mailbox not empty. wait for signal(unlock)\n", args->tname);
        pthread_cond_wait(&flag, &lock);
        printf("COUNT_%s: I have the lock\n", args->tname);
    }
    mailbox = args;
    printf("COUNT_%s: raising flag\n", args->tname);
    pthread_cond_signal(&flag);
    sleep(1);
    printf("COUNT_%s: unlocking box\n", args->tname);
    pthread_mutex_unlock(&lock);

    return NULL;
}
