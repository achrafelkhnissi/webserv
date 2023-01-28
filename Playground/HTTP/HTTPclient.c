#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// include the header file socket
#include <sys/socket.h>
#include <sys/types.h>

// include the header file for the internet address
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int ac, char** av)
{

    if (ac != 2) {
        fprintf(stderr, "Usage: %s <ip address>\n", av[0]);
        return EXIT_FAILURE;
    }

    char* ip_address = av[1];

    // create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the address
    struct sockaddr_in remote_address = {
        .sin_family = AF_INET,
        .sin_port = htons(80),
        .sin_addr.s_addr = inet_addr(ip_address)
    };

    // connect to the remote address
    connect(client_socket, (struct sockaddr*) &remote_address, sizeof(remote_address));

    // request to send a message
    char request[] = "GET / HTTP/1.1\r\n\r\n";
    send(client_socket, request, sizeof(request), 0);

    // receive data from the server
    char response[4096];
    recv(client_socket, &response, sizeof(response), 0);

    // print out the server's response
    printf("Response from the server: %s\n", response);

    // close the socket
    close(client_socket);

    return EXIT_SUCCESS;
}
