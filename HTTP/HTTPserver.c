#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// include the header file socket
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

int main() {

    // open a file to serve
    FILE* html_data = fopen("srcs.html", "r");

    char response_data[1024];
    fgets(response_data, 1024, html_data);

    char http_header[2048] = "HTTP/1.1 200 OK\r\n\n";
    strcat(http_header, response_data);

    printf("%s\n\n", http_header);

    // create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the address
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(8001),
        .sin_addr.s_addr = INADDR_ANY
    };

    // bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // listen for connections
    listen(server_socket, 5);

    while (1) {
        // accept a connection
        int client_socket = accept(server_socket, NULL, NULL);

        printf("Connection accepted [%d]\n", client_socket);

        // send the message
        send(client_socket, http_header, sizeof(http_header), 0);

        // close the connection
        close(client_socket);
    }

    // close the socket
    close(server_socket);

    return EXIT_SUCCESS;
}