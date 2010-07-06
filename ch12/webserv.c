/**
 * webserv.c
 *
 * GETコマンドのみをサポートするWebSever
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void read_til_crnl(FILE*);
void process_rq(char*, int);
void header(FILE*, char*);
void cannot_do(int);
void do_404(char*, int);
int is_dir(char*);
int not_exist(char*);
void do_ls(char*, int);
char *file_type(char*);
int ends_in_cgi(char*);
void do_exec(char*, int);
void do_cat(char*, int);

/**
 * \r\nを検出するまで飛ばす
 */
void read_til_crnl(FILE *fp)
{
    char buf[BUFSIZ];
    while( fgets(buf, BUFSIZ, fp) != NULL && strcmp(buf, "\r\n") != 0 )
        ;
}

/**
 * リクエストが要求してきた処理を実行し、fdにレスポンスを書き込む
 * rq -> GET /foo/bar.html HTTP/1.0
 */
void process_rq(char *rq, int fd)
{
    char cmd[BUFSIZ], arg[BUFSIZ];

    /* 親プロセスなら即リターン */
    if ( fork() != 0 )
        return;
    
    strcpy(arg, "./");
    if ( sscanf(rq, "%s%s", cmd, arg+2) != 2 ) /* +2 は "./" の分 */
        return;

    if ( strcmp(cmd, "GET") != 0 )
        cannot_do(fd);
    else if ( not_exist(arg) )
        do_404(arg, fd);
    else if ( is_dir(arg) )
        do_ls(arg, fd);
    else if ( ends_in_cgi(arg) )
        do_exec(arg, fd);
    else
        do_cat(arg, fd);
}

/**
 * response headerの生成
 * content_typeがNULLならContent-type行を送らない
 */
void header(FILE* fp, char *content_type)
{
    fprintf(fp, "HTTP/1.0 200 OK\r\n");
    if ( content_type )
        fprintf(fp, "Content-type: %s\r\n", content_type);
}
/**
 * 実装されていないコマンド
 */
void cannot_do(int fd)
{
    FILE *fp = fdopen(fd, "w");

    fprintf(fp, "HTTP/1.0 501 Not Implemented\r\n");
    fprintf(fp, "Content-type: text/plain\r\n");
    fprintf(fp, "\r\n");

    fprintf(fp, "That command is not yet implemented\r\n");
    fclose(fp);
}
/**
 * 指定されたオブジェクトがない
 */
void do_404(char *item, int fd)
{
    FILE *fp = fdopen(fd, "w");

    fprintf(fp, "HTTP/1.0 404 Not Found\r\n");
    fprintf(fp, "Content-type: text/plain\r\n");
    fprintf(fp, "\r\n");

    fprintf(fp, "The item you requested: %s\r\nis not found\r\n", item);
    fclose(fp);
}

int is_dir(char *f)
{
    struct stat info;
    return ( stat(f, &info) != -1 && S_ISDIR(info.st_mode) );
}

int not_exist(char *f)
{
    struct stat info;
    return ( stat(f, &info) == -1 );
}

void do_ls(char *dir, int fd)
{
    FILE *fp;

    fp = fdopen(fd, "w");
    header(fp, "text/plain");
    fprintf(fp, "\r\n");
    fflush(fp);

    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execlp("ls", "ls", "-l", dir, NULL);
    perror(dir);
    exit(1);
}

char *file_type(char *f)
{
    char *cp;
    if ( (cp = strrchr(f, '.')) != NULL )
        return cp + 1; // 拡張子名の先頭
    return "";
}

int ends_in_cgi(char *f)
{
    return ( strcmp( file_type(f), "cgi" ) == 0 );
}

void do_exec(char *prog, int fd)
{
    FILE *fp;

    fp = fdopen(fd, "w");
    header(fp, NULL);
    fflush(fp);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execl(prog, prog, NULL);
    perror(prog);
}

void do_cat(char *f, int fd)
{
    char *extension = file_type(f);
    char *content = "text/plain";
    FILE *fpsock, *fpfile;
    int c;

    if ( strcmp(extension, "html") == 0 )
        content = "text/html";
    else if ( strcmp(extension, "gif") == 0 )
        content = "image/gif";
    else if ( strcmp(extension, "jpg") == 0 )
        content = "image/jpeg";
    else if ( strcmp(extension, "jpeg") == 0 )
        content = "image/jpeg";

    fpsock = fdopen(fd, "w");
    fpfile = fopen(f, "r");
    if ( fpsock != NULL && fpfile != NULL )
    {
        /* output header */
        header(fpsock, content);
        fprintf(fpsock, "\r\n");
        /* output request file */
        while( (c = getc(fpfile)) != EOF )
            putc(c, fpsock);
        fclose(fpfile);
        fclose(fpsock);
    }
    exit(0);
}

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
        fd = accept(sock, NULL, NULL); /* 読み書き両用のfd */
        fpin = fdopen(fd, "r");

        fgets(request, BUFSIZ, fpin);
        printf("got a call: request = %s", request);
        read_til_crnl(fpin);

        process_rq(request, fd);
        fclose(fpin);
    }
}


