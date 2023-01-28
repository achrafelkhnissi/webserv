#include "common.h"

#define SERVER_PORT 80

int main(int argc, char **argv)
{
    int socketfd, n;
    int sendbytes;
    struct  sockaddr_in servaddr;
    char    sendline[MAXLINE], recvline[MAXLINE];

    if (argc != 2)
        err_n_die("Usage: %s <Server IP>", argv[0]);

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        err_n_die("inet_pton error for %s", argv[1]);

    if (connect(socketfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
        err_n_die("connect failed!");

    // We're connected, so prepare to send the HTTP request
    sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);

    // Send the HTTP request
    if (write(socketfd, sendline, sendbytes) != sendbytes)
        err_n_die("write error");

    memset(recvline, 0, MAXLINE);
    // Read the HTTP response
    while ((n = read(socketfd, recvline, MAXLINE - 1)) > 0) {
        printf("%s", recvline);
        // recvline[n] = 0;    // null terminate
        // if (fputs(recvline, stdout) == EOF)
        //     err_n_die("fputs error");
    }

    if (n < 0)
        err_n_die("read error");

    exit(0);
}
