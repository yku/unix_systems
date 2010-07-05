/**
 * webserv.c
 *
 * GETコマンドのみをサポートするWebSever
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char** argv)
{
    int sock, fd;
    FILE *fpin;
    char request[BUFSIZ];

    if ( argc == 1 ) {
        fprintf(stderr, "usage: ws portnum\n");
        exit(1);
    }

    sock = make_server_socket( atoi(argv[1]) );
    if ( sock == -1 )
        exit(2);

    while(1){
        fd = accept(sock, NULL, NULL);
        fpin = fdopen(fd, "r");

        fgets(request, BUFSIZ, fpin);
        printf("got a call: request = %s", request);
        read_til_crnl(fpin);

        process_rq(request, fd);
        fclose(fpin);
    }
}


