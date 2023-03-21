#include <stdio.h>
#include <stdlib.h>

// include socket header files
#include <sys/types.h>
#include <sys/socket.h>

// include internet header files
#include <netinet/in.h>

#include <unistd.h>

int main()
{
    // create a socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;
     
    // connect to the server
    int connection_status = connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status == -1) {
        fprintf(stderr, "There was an error making a connection to the remote socket\n");
    }

    // receive data from the server
    char server_response[256];
    recv(socket_fd, &server_response, sizeof(server_response), 0);

    // print out the server's response
    printf("The server sent the data: %s\n", server_response);

    // and then close the socket
    close(socket_fd);

    return EXIT_SUCCESS;
}
