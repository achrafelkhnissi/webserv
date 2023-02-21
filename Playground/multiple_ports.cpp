//
// Created by Fathiyat olatokunbo Jamia on 2/19/23.
//
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CONNECTIONS 5

using namespace std;

int main(int argc, char *argv[])
{
    // Create socket for port 80
    int server_socket1 = socket(AF_INET, SOCK_STREAM, 0);

    // Create socket for port 8080
    int server_socket2 = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket1 == -1 || server_socket2 == -1) {
        cerr << "Could not create socket" << endl;
        return 1;
    }

    // Bind port 80
    struct sockaddr_in server_address1;
    server_address1.sin_family = AF_INET;
    server_address1.sin_addr.s_addr = INADDR_ANY;
    server_address1.sin_port = htons(1337);

    if (bind(server_socket1, (struct sockaddr *)&server_address1, sizeof(server_address1)) == -1) {
        cerr << "Could not bind to port 1337" << endl;
        return 1;
    }

    // Bind port 8080
    struct sockaddr_in server_address2;
    server_address2.sin_family = AF_INET;
    server_address2.sin_addr.s_addr = INADDR_ANY;
    server_address2.sin_port = htons(42);

    if (bind(server_socket2, (struct sockaddr *)&server_address2, sizeof(server_address2)) == -1) {
        cerr << "Could not bind to port 42" << endl;
        return 1;
    }

    // Listen on both ports
    listen(server_socket1, MAX_CONNECTIONS);
    listen(server_socket2, MAX_CONNECTIONS);

    cout << "Web server listening on ports 1337 and 42..." << endl;

    while (true) {
        // Accept incoming connections on both sockets
        int client_socket1 = accept(server_socket1, NULL, NULL);
        int client_socket2 = accept(server_socket2, NULL, NULL);

        // Read request from client
        char buffer[1024] = {0};
        read(client_socket1, buffer, 1024);

        // Send response to client
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!\r\n";
        write(client_socket1, response, strlen(response));

        // Close connection
        close(client_socket1);
        close(client_socket2);
    }

    // Close server sockets
    close(server_socket1);
    close(server_socket2);

    return 0;
}
