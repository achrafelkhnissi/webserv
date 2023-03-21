#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

const int PORT = 8080;
const int BUFFER_SIZE = 4096;

const char* filePath = "/Users/fathjami/Desktop/webserv/www/media/image/myimage.jpg";
//	const char* videoFilePath = "/Users/ael-khni/CLionProjects/webserv/www/media/video/video.mp4";


void sendFile(int socket, const char* filePath) {
    ifstream fileStream(filePath, ios::in | ios::binary);

    if (!fileStream) {
        cerr << "Unable to open file" << endl;
        return;
    }

    // Calculate the size of the file
    fileStream.seekg(0, ios::end);
    int fileSize = fileStream.tellg();
    fileStream.seekg(0, ios::beg);

    // Create the HTTP response header with the Content-Length field
    ostringstream responseHeader;
    responseHeader << "HTTP/1.1 200 OK\r\n";
    responseHeader << "Server: Webserv/1.0\r\n";
    responseHeader << "Content-Type: image/jpeg\r\n";
    responseHeader << "Content-Length: " << to_string(fileSize) << "\r\n";
    responseHeader << "Connection: Keep-Alive\r\n";
    responseHeader << "\r\n";

    // Send the HTTP response header
    const string& response = responseHeader.str();
    send(socket, response.c_str(), response.size(), 0);

    // Send the file data
    char buffer[BUFFER_SIZE];
    int bytesSent = 0;
    while (fileStream) {
        fileStream.read(buffer, BUFFER_SIZE);
        int bytesRead = fileStream.gcount();
        int ret = send(socket, buffer, bytesRead, MSG_DONTWAIT);
        if (ret < 0) {
            std::string err = strerror(errno);
            ::perror("send");

            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                cout << "EAGAIN || EWOULDBLOCK" << endl;
                break ;
            }
            std::cout << "ret: " << ret << std::endl;
            cerr << "Error sending file" << endl;
            break;
        }

        bytesSent += bytesRead;
    }

    fileStream.close();
}

int main(int argc, char const *argv[]) {
    int serverSocket, newSocket, maxSocket, valRead, activity;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    int optionValue_ = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optionValue_, sizeof(optionValue_)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(argv[1] ? atoi(argv[1]) : PORT);

    if (bind(serverSocket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    fd_set readfds, writefds, errorfds;

    // Add the server socket to the read and error sets
    FD_SET(serverSocket, &readfds);
    FD_SET(serverSocket, &errorfds);

// Set the maximum socket descriptor
    maxSocket = serverSocket;

// Add the server socket to the read and error sets
    FD_SET(serverSocket, &readfds);
    FD_SET(serverSocket, &errorfds);

// Set the maximum socket descriptor
    maxSocket = serverSocket;

    while (true) {
        // Wait for activity on the sockets
        activity = select(maxSocket + 1, &readfds, &writefds, &errorfds, NULL);

        if (activity < 0 && errno != EINTR) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        // Check for incoming connection
        if (FD_ISSET(serverSocket, &readfds)) {
            if ((newSocket = accept(serverSocket, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            cout << "New connection, socket fd is " << newSocket << endl;

            // Add the new socket to the read and error sets
            FD_SET(newSocket, &readfds);
            FD_SET(newSocket, &errorfds);

            // Update the maximum socket descriptor
            if (newSocket > maxSocket) {
                maxSocket = newSocket;
            }
        }

        // Check for incoming data on sockets
        for (int i = serverSocket + 1; i <= maxSocket; i++) {
            if (FD_ISSET(i, &readfds)) {
                valRead = read(i, buffer, BUFFER_SIZE);
                if (valRead == 0) {
                    // Client disconnected, remove the socket from all sets
                    cout << "Client disconnected, socket fd is " << i << endl;
                    close(i);
                    FD_CLR(i, &readfds);
                    FD_CLR(i, &writefds);
                    FD_CLR(i, &errorfds);
                } else {
                    // Process the received data
                    cout << "Received data from client, socket fd is " << i << endl;
                    sendFile(i, filePath);
                }
            }
        }

        // Clear the file descriptor sets for the next iteration
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&errorfds);

        // Add the sockets to the sets
        FD_SET(serverSocket, &readfds);
        FD_SET(serverSocket, &errorfds);

        for (int i = serverSocket + 1; i <= maxSocket; i++) {
            FD_SET(i, &readfds);
            FD_SET(i, &errorfds);
        }
    }

    return 0;
}