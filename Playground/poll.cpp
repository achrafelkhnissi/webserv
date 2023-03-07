//
// Created by Achraf El khnissi on 2/16/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <fcntl.h>

#define MAX_CONNECTIONS 1000
#define BUFFER_SIZE 1024

int create_server_socket(int port) {
    struct sockaddr_in server_addr;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }
    // Set the server socket to non-blocking mode
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }
    if (listen(server_socket, 5) == -1) {
        perror("listen");
        exit(1);
    }
    return server_socket;
}

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    int n = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (n == -1) {
        perror("read");
        exit(1);
    }
    if (n == 0) {
        // Connection closed
        close(client_socket);
        return;
    }
    buffer[n] = '\0';
    printf("Received request: %s\n", buffer);

    const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
    write(client_socket, response, strlen(response));
    close(client_socket);
}

int main() {
    int server_socket = create_server_socket(1337);
    struct pollfd pollfds[MAX_CONNECTIONS];
    int client_sockets[MAX_CONNECTIONS];
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        pollfds[i].fd = -1;
        client_sockets[i] = -1;
    }
    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN;

    while (1) {
        int ready = poll(pollfds, MAX_CONNECTIONS, -1);
        if (ready == -1) {
            perror("poll");
            break;
        }
        if (pollfds[0].revents & POLLIN) {
            // New connection
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
            if (client_socket == -1) {
                perror("accept");
            } else {
                int i;
                for (i = 1; i < MAX_CONNECTIONS; i++) {
                    if (client_sockets[i] == -1) {
                        pollfds[i].fd = client_socket;
                        pollfds[i].events = POLLIN;
                        client_sockets[i] = client_socket;
                        break;
                    }
                }
                if (i == MAX_CONNECTIONS) {
                    fprintf(stderr, "Too many connections\n");
                    close(client_socket);
                }
            }
            ready--;
        }
        for (int i = 1; i < MAX_CONNECTIONS && ready > 0; i++) {
            if (pollfds[i].revents & POLLIN) {
                handle_request(client_sockets[i]);
                pollfds[i].fd = -1;
                pollfds[i].events = POLLIN;
                client_sockets[i] = -1;
                ready--;
            }
        }
    }
    close(server_socket);
    return 0;
}
