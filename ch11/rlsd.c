/*
 * rlsd.c
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include <stdlib.h>

#define PORTNUM 15000
#define HOSTLEN 256
#define oops(msg) { perror(msg); exit(1); }

void sanitize(char* str);

int main(int argc, char** argv)
{
    struct sockaddr_in saddr;
    struct hostent *hp;
    char hostname[HOSTLEN];
    int sock_id, sock_fd;
    FILE *sock_fpi, *sock_fpo;
    FILE *pipe_fp;
    char dirname[BUFSIZ];
    char command[BUFSIZ];

    int dirlen, c;

    sock_id = socket( PF_INET, SOCK_STREAM, 0 );
    if ( sock_id == -1 )
        oops("socket");

    bzero((void *)&saddr, sizeof(saddr));
    gethostname( hostname, HOSTLEN );
    hp = gethostbyname( hostname );
    bcopy( (void *)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length );
    saddr.sin_port = htons( PORTNUM );
    saddr.sin_family = AF_INET;
    if ( bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0 )
        oops( "bind" );

    if ( listen(sock_id, 1) != 0 )
        oops("listen");

    while(1){
        sock_fd = accept(sock_id, NULL, NULL);
        if ( sock_fd == -1 )
            oops("accept");
        
        if ( (sock_fpi = fdopen( sock_fd, "r" )) == NULL )
            oops("fdopen reading");
        /* get a directory name from sock_fpi */
        if ( fgets(dirname, BUFSIZ - 5, sock_fpi) == NULL )
            oops("reading dirname");
        sanitize(dirname);

        if ( (sock_fpo = fdopen(sock_fd, "w")) == NULL )
            oops("fdopen writing");

        sprintf(command, "ls %s", dirname);
        if ( (pipe_fp = popen(command, "r")) == NULL )
            oops("popen");

        /* transfer data to socket from ls */
        while( (c = getc(pipe_fp)) != EOF)
            putc(c, sock_fpo);
        pclose(pipe_fp);
        fclose(sock_fpo);
        fclose(sock_fpi);
    }
}

void sanitize(char *str)
{
    char *src, *dest;
    src = dest = str;

    while( *src++ )
        if ( *src == '/' || isalnum(*src) )
            *dest++ = *src;
    *dest = '\0';
}
