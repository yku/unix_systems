/**
 * rls <host> <directory>
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define oops(msg) { perror(msg); exit(1); }
#define PORTNUM 15000

int main(int argc, char** argv)
{
    struct sockaddr_in servadd;
    struct hostent *hp;
    int sock_id, sock_fd;
    char buf[BUFSIZ];
    int n_read;

    if ( argc != 3 ) exit(1);

    /* create socket */
    sock_id = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock_id == -1 )
        oops( "socket" );
    
    /* connect server */
    bzero( &servadd, sizeof(servadd) );
    hp = gethostbyname( argv[1] );
    if ( hp == NULL )
        oops( argv[1] );
    bcopy( hp->h_addr, (struct sockaddr *)&servadd.sin_addr, hp->h_length );
    servadd.sin_port = htons( PORTNUM );
    servadd.sin_family = AF_INET;

    if ( connect( sock_id, (struct sockaddr *)&servadd, sizeof(servadd) ) != 0 )
        oops( "connect" );

    /* send a directory name and read contents */
    if ( write( sock_id, argv[2], strlen( argv[2] ) ) == -1 )
        oops( "write" );
    if ( write( sock_id, "\n", 1 ) == -1 )
        oops( "write" );

    while ( ( n_read = read( sock_id, buf, BUFSIZ ) ) > 0 )
        if ( write( 1, buf, n_read ) == -1 )
            oops( "write" );
    close( sock_id );
}

