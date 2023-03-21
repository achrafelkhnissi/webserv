#include "common.h"

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    int sendbytes;
    struct  sockaddr_in servaddr;
    char    buff[MAXLINE + 1], recvline[MAXLINE + 1];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family         = AF_INET;
    servaddr.sin_addr.s_addr    = htonl(INADDR_ANY);
    servaddr.sin_port           = htons(SERVER_PORT);

    if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
        err_n_die("bind error"); 
    
    if ((listen(listenfd, 10)) < 0)
        err_n_die("listen error");

    for ( ; ; ) {
        struct sockaddr_in addr;
        socklen_t len; // = sizeof(addr);
        char client_address[MAXLINE + 1];

        printf("Waiting for a connection on port %d\n\n", SERVER_PORT);
        fflush(stdout);

        if ((connfd = accept(listenfd, (SA *) &addr, &len)) < 0)
            err_n_die("accept error");

        inet_ntop(AF_INET, &(addr.sin_addr), client_address, MAXLINE);
        printf("Connection from %s\n\n", client_address);

        memset(recvline, 0, MAXLINE);

        while ((n = read(connfd, recvline, MAXLINE - 1)) > 0) {

            // fprintf(stdout, "\n%s\n\n%s", bin2hex(recvline, n), recvline);
            printf("%s", recvline);

            if (recvline[n - 1] == '\n')
                break;
            
            memset(recvline, 0, MAXLINE);
        }

        if (n < 0)
            err_n_die("read error");

        snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200 OK\r\n\r\nHello");

        write(connfd, (char*)buff, strlen(buff));
        
        close(connfd);
    }
        

    exit(0);
}
