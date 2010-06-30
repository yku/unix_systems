/*
 * timeclnt.c
 * timeserv.cのクライアント
 * usage: timeclnt <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define oops(msg) { perror(msg); exit(1); }


int main(int argc, char** argv)
{
    struct sockaddr_in servadd;
    struct hostent *hp;
    char message[BUFSIZ];
    int sock_id, sock_fd;
    int messlen;

    /* Step1. ソケットを作成 */
    sock_id = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_id == -1)
        oops("socket");

    /* Step2. Serverに接続 */
    bzero((void *)&servadd, sizeof(servadd));
    hp = gethostbyname(argv[1]);
    bcopy(hp->h_addr, (struct sockaddr *)&servadd.sin_addr, hp->h_length);
    servadd.sin_port = htons(atoi(argv[2]));
    servadd.sin_family = AF_INET;

    if ( connect(sock_id, (struct sockaddr *)&servadd, sizeof(servadd)) != 0 )
        oops("connect");

    /* Step3. Serverからデータを受け取って電話を切る */
    messlen = read(sock_id, message, BUFSIZ);
    if ( messlen == -1 ) 
        oops("read");
    if ( write(1, message, messlen) != messlen )
        oops("write");
    close(sock_id);
}
