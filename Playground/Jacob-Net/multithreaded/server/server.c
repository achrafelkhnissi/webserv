/*
    ! Problem: This server is not multithreaded, so it can only handle one client at a time.
    ! Solution: Make it multithreaded.

    source: https://www.youtube.com/watch?v=Pg_4Jz8ZIH4&list=PL9IEJIKnBJjH_zM5LnovnoaKlXML5qh17&index=5
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>

#define SERVER_PORT 8989
#define BUFFER_SIZE 4096
#define SOCKET_ERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void handle_connection(int client_socket);
int check(int exp, const char *msg);

int main(int argc, char** argv)
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr, client_addr;

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
        "Failed to create socket");

    // initialize the address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // htonl(INADDR_ANY);

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)),
        "Failed to bind socket");

    check(listen(server_socket, SERVER_BACKLOG),
        "Failed to listen on socket");

    while(true) {
        printf("Waiting for connection...\n");

        addr_size = sizeof(SA_IN);

        check((client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size)),
            "Failed to accept connection");
        
        printf("Connected!\n");

        handle_connection(client_socket);

    } // end while

    return 0;
}

void handle_connection(int client_socket)
{
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int msg_size = 0;
    char actualpath[PATH_MAX + 1];

    // read the client's message - the name of the file to read
    while((bytes_read = read(client_socket, buffer + msg_size, sizeof(buffer) - msg_size - 1)) > 0) {
        msg_size += bytes_read;
        if (msg_size > BUFFER_SIZE - 1 || buffer[msg_size - 1] == '\n')
            break;
    }

    check(bytes_read, "recv error");
    buffer[msg_size - 1] = 0 ;

    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    // validity check
    if (realpath(buffer, actualpath) == NULL) {
        printf("Invalid path: %s\n", buffer);
        close(client_socket);
        return;
    }

    // read file and send its contents to the client
    FILE* fp = fopen(actualpath, "r");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", buffer);
        close(client_socket);
        return;
    }
    // TODO: remove this
    sleep(1); // this gonna make this server slow

    // read the file and send it to the client
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        // check(write(client_socket, buffer, bytes_read), "send error");
        printf("sending %zu bytes\n", bytes_read);
        write(client_socket, buffer, bytes_read);
    }

    close(client_socket);
    fclose(fp);

    printf("Closing connection\n\n");
}

int check(int exp, const char *msg)
{
    if (exp == SOCKET_ERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}
