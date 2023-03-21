//
//  main.cpp
//  webserv
//
//  Created by Achraf El khnissi on 1/12/23.
//  Copyright © 2023 Achraf El khnissi. All rights reserved.
//

#include <iostream>
#include <sys/socket.h> // getaddrinfo
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>


int main(int ac, char** av) {

    (void)ac;
    (void)av;

    /* Server information */
    const char* host = nullptr; //"google.com";
    const char* port = "8080"; // "80";
    int r, sockfd, clientfd;
    struct addrinfo hints = {}, *server;
    struct sockaddr client_address = {};
    socklen_t client_len;

    const int buffer_size = 1024;
    char buffer[buffer_size];
    const char* http_data =
            "HTTP/1.1 200 OK\r\n"
            "Connection: close\r\n"
            "Content-Type: text/html\r\n\r\n"
            "<h1>Hello from you server!</h1>";

    /* prep the hints structure */
    std::cout << "Configuring host... ";

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;          /* IPv4 */
    hints.ai_socktype = SOCK_STREAM;    /* TCP */

    /* fill the resource structure */
    r = getaddrinfo(host, port, &hints, &server);
    if ( r != 0 ) {
        perror("Failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "done" << std::endl;

    /* create the socket */
    std::cout << "Assign a socket... ";
    sockfd = socket(
            server->ai_family,      /* domain, TCP here */
            server->ai_socktype,    /* type, stream */
            server->ai_protocol     /* protocol, IP */
            );
    if ( sockfd == -1 ) {
        perror("Failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "done" << std::endl;

    /* bind - name the socket (replaced with connect for the client) */
    std::cout << "Binding socket... ";
    r = bind(sockfd,
             server->ai_addr,
             server->ai_addrlen
             );
    if ( r == -1 ) {
        perror("Failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "done" << std::endl;

    /* listen for incoming connections */
    std::cout << "Listening... ";
    r = listen(sockfd, 1);
    if ( r == -1 ) {
        perror("Failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "done" << std::endl;

    /* accept a new connection */
    std::cout << "Accepting new connection ";
    client_len = sizeof(client_address);
    clientfd = accept(sockfd,
                      &client_address,
                      &client_len
                      );
    if ( clientfd == -1 ) {
        perror("Failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "on file descriptor " << clientfd << std::endl;

    /* client is now connected */
    r = recv(clientfd, buffer, buffer_size, 0);
    if ( r > 0 ) {
        std::cout << "\nReceived " << r << " bytes:\n---\n";
        std::cout << buffer;
    }

    /* send the response */
    r = send(clientfd, http_data, strlen(http_data), 0);
    if ( r < 1 ) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Sent " << r << " bytes" << std::endl;

    /* free allocated memory */
    freeaddrinfo(server);

    /* close the opened socket descriptor */
    close(sockfd);
    close(clientfd);

    return 0;
}

/*
 * TCP Client
 *  1. getaddrinfo() to obtain information about the host.
 *  2. socket() to create the socket, the communications endpoint. A socket file descriptor is returned
 *          which is used to reference the socket elsewhere in the code.
 *  3. connect() to connect the socket with a host. At this point, the connection is established.
 *  4. send() and recv() communicate with a host.
 *  5. close() to close the socket file descriptor. The connection is terminated.
 *
 * TCP Server
 *  1. getaddrinfo() to configure the server's address, port, protocol and other details.
 *  2. socket() to create a new communication endpoint.
 *  3. bind() binds the socket to the address and port provided by the getaddrinfo() function.
 *      at this point, the server is configured to deal with incoming connections.
 *  4. listen() to monitor the open socket.
 *  5. accept(): when a connection is made, the accept() function to create a new socket to communicate with the client.
 *  6. send() and recv() used to exchange data.
 *  7. close() close the open socket when the server is done serving.
 */