// include socket header files
#include <sys/types.h>
#include <sys/socket.h>

// include internet header files
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

int main()
{
    // create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
     
    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // listen for connections
    listen(server_socket, 5);

    // accept a connection
    int client_socket = accept(server_socket, NULL, NULL);

    // send the message
    char server_message[256] = "You have reached the server!";
    send(client_socket, server_message, sizeof(server_message), 0);

    // close the socket
    close(server_socket);

    return EXIT_SUCCESS;
}
