
/*
    ! problem: the server is not able to handle multiple clients at the same time
    !          because the server uses synchronous I/O calls
    !           - the server is blocked while waiting for a client to send a message
    !           - the server is blocked while waiting for a client to receive a message
    !           - the server is blocked while waiting for a client to close the connection
    ! solution: use asynchronous I/O calls

    note: the server will only be able to handle up to FD_SETSIZE clients at the same time
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>

#include <pthread.h>

#define SERVER_PORT 8989
#define BUFFER_SIZE 4096
#define SOCKET_ERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void* handle_connection(int client_socket);
int check(int exp, const char *msg);
int accept_new_connection(int server_socket);
int setup_server(short port, int backlog);

int main(int argc, char** argv)
{
    int server_socket = setup_server(SERVER_PORT, SERVER_BACKLOG);

    fd_set current_sockets, ready_sockets;
    int max_socket_so_far = 0; // to prevent iterating over all the sockets FD_SETSIZE times

    // initialize the set of active sockets
    FD_ZERO(&current_sockets); // clears the set
    FD_SET(server_socket, &current_sockets); // adds the server socket to the set
    max_socket_so_far = server_socket;

    while (true) {
        // because select() is destructive
        ready_sockets = current_sockets; // select() modifies the set, so we need to make a copy

        // call select() and wait for an activity on one of the sockets
        if (select(max_socket_so_far, &ready_sockets, NULL, NULL, NULL) < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        // printf("Waiting for connection...\n");

        // check which socket is ready
        for (int i = 0; i <= max_socket_so_far; i++) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == server_socket) {
                    // a new connection request
                    int client_socket = accept_new_connection(server_socket);
                    FD_SET(client_socket, &current_sockets);

                    if (client_socket > max_socket_so_far)
                        max_socket_so_far = client_socket;

                } else {
                    // a message from a client
                    handle_connection(i);
                    FD_CLR(i, &current_sockets);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

int setup_server(short port, int backlog)
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr;

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
        "Failed to create socket");

    // initialize the address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // htonl(INADDR_ANY);

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)),
        "Failed to bind socket");

    check(listen(server_socket, backlog),
        "Failed to listen on socket");

    return server_socket;
}

int accept_new_connection(int server_socket)
{
    int client_socket;
    SA_IN client_addr;
    socklen_t addr_size = sizeof(SA_IN);

    check((client_socket = accept(server_socket, (SA*)&client_addr, &addr_size)),
        "Failed to accept connection");

    printf("Connected!\n");

    return client_socket;
}

void* handle_connection(int client_socket)
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
        return NULL;
    }

    // read file and send its contents to the client
    FILE* fp = fopen(actualpath, "r");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", buffer);
        close(client_socket);
        return NULL;
    }
    // TODO: delete it
    // sleep(1); // this gonna make the non threaded server very slow!

    // read the file and send it to the client
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        // check(write(client_socket, buffer, bytes_read), "send error");
        printf("sending %zu bytes\n", bytes_read);
        write(client_socket, buffer, bytes_read);
    }

    close(client_socket);
    fclose(fp);

    printf("Closing connection\n\n");
    return NULL;
}

int check(int exp, const char *msg)
{
    if (exp == SOCKET_ERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}
