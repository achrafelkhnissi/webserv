//#include <iostream>
//#include <fstream>
//#include <string>
//#include <cstring>
//#include <cstdlib>
//#include <unistd.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//
//#define PORT 4242
//#define BUFFER_SIZE 1024
//
//using namespace std;
//
//void sendFile(int socket, const char* filePath) {
//    ifstream fileStream(filePath, ios::in | ios::binary);
//
//    if (!fileStream) {
//        cerr << "Unable to open file" << endl;
//        return;
//    }
//
//    char buffer[BUFFER_SIZE];
//    while (fileStream) {
//        fileStream.read(buffer, BUFFER_SIZE);
//        int bytesRead = fileStream.gcount();
//        send(socket, buffer, bytesRead, 0);
//    }
//
//    fileStream.close();
//}
//
//int main(int argc, char const *argv[]) {
//    int serverSocket, newSocket, valRead;
//    struct sockaddr_in address;
//    int opt = 1;
//    int addrlen = sizeof(address);
//    char buffer[BUFFER_SIZE] = {0};
//    const char* videoFilePath = "/Users/ael-khni/CLionProjects/webserv/www/media/image/myimage.jpg";
//    char* response = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: 1513071\r\n\r\n";
////	const char* videoFilePath = "/Users/ael-khni/CLionProjects/webserv/www/media/video/video.mp4";
////	char* response = "HTTP/1.1 200 OK\r\nContent-Type: video/mp4\r\nContent-Length: 1570024\r\n\r\n";
//
//    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons(PORT);
//
//    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address))<0) {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//
//    if (listen(serverSocket, 3) < 0) {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//
//    while (true) {
//        if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
//            perror("accept");
//            exit(EXIT_FAILURE);
//        }
//
////        valRead = read(newSocket, buffer, BUFFER_SIZE);
//        recv(newSocket, buffer, BUFFER_SIZE, 0);
//        cout << buffer << endl;
//
//        send(newSocket, response, strlen(response), 0);
//        sendFile(newSocket, videoFilePath);
//
//        close(newSocket);
//    }
//
//    return 0;
//}

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <fcntl.h>
#include <vector>
#include <poll.h>


#define PORT 4242
//#define PORT 1337
#define BUFFER_SIZE 1024

using namespace std;

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
//   stringstream responseHeader;
    responseHeader << "HTTP/1.1 200 OK\r\n";
    responseHeader << "Content-Type: image/jpeg\r\n";
//	responseHeader << "Content-Type: image/jpeg\r\n";
    responseHeader << "Content-Length: " << to_string(fileSize) << "\r\n";
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
        send(socket, buffer, bytesRead, 0);
        bytesSent += bytesRead;
    }
    std::cout << "Bytes sent: " << bytesSent << std::endl;
    fileStream.close();
}

//int main(int argc, char const *argv[]) {
//    int serverSocket, newSocket, valRead;
//    struct sockaddr_in address;
//    int opt = 1;
//    int addrlen = sizeof(address);
//    char buffer[BUFFER_SIZE] = {0};
////    const char* videoFilePath = "/Users/ael-khni/CLionProjects/webserv/www/media/image/myimage.jpg";
//    const char *videoFilePath = "../www/media/image/myimage.jpg";
//
//
//    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//    if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) < 0) {
//        perror("fcntl");
//        exit(EXIT_FAILURE);
//    }
//
//    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//
//
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons(PORT);
//
//    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address))<0) {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//
//    if (listen(serverSocket, 3) < 0) {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//
//    while (true) {
//        if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
//            perror("accept");
//            exit(EXIT_FAILURE);
//        }
//
//        recv(newSocket, buffer, BUFFER_SIZE, 0);
//        cout << buffer << endl;
//
//        sendFile(newSocket, videoFilePath);
//
//        close(newSocket);
//    }
//
//    return 0;
//}

#define CHUNK_SIZE 1024

int main(int argc, char const *argv[]) {
    int serverSocket, newSocket, valRead;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *videoFilePath = "../www/media/image/myimage.jpg";

    // Create the server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set the socket as non-blocking
    int flags = fcntl(serverSocket, F_GETFL, 0);
    fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

    // Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Create a pollfd struct for the server socket
    struct pollfd fds[1];
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;

    // Create a vector to store connected client sockets
    vector<int> clientSockets;
    while (true) {
        // Wait for events on the server socket and client sockets
        int pollCount = poll(fds, clientSockets.size() + 1, -1);
        if (pollCount == -1) {
            perror("poll failed");
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
            // Accept a new connection
            if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            // Add the new socket to the vector
            clientSockets.push_back(newSocket);
            // Add the new socket to the pollfd struct
//            fds[clientSockets.size()].fd = newSocket;
//            fds[clientSockets.size()].events = POLLIN;

            flags = fcntl(newSocket, F_GETFL, 0);
            fcntl(newSocket, F_SETFL, flags | O_NONBLOCK);

            printf("New connection, socket fd is %d, ip is : %s, port : %d\n" , newSocket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

// Send the file to the client
            FILE* file = fopen(videoFilePath, "rb");
            if (file == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            // Read the video file in chunks and send to connected clients
            int chunkSize = CHUNK_SIZE;
            int bytesRead = 0;
            while ((bytesRead = fread(buffer, sizeof(char), chunkSize, file)) > 0) {
                // Loop through all connected clients and send the data
                for (int i = 0; i < clientSockets.size(); i++) {
                    int clientSocket = clientSockets[i];
                    send(clientSocket, buffer, bytesRead, 0);
                }
                memset(buffer, 0, sizeof(buffer));
            }
            fclose(file);
        }

    }
   return 0;
    }

//    while (true) {
//// Wait for activity on the sockets using poll()
//        struct pollfd fds[1];
//        fds[0].fd = serverSocket;
//        fds[0].events = POLLIN;
//        int numSocketsReady = poll(fds, 1, 1000);
//
//        if (numSocketsReady < 0) {
//            perror("poll failed");
//            exit(EXIT_FAILURE);
//        } else if (numSocketsReady == 0) {
//            continue;
//        }
//
//// If there is activity on the server socket, accept a new connection
//        if (fds[0].revents & POLLIN) {
//            if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
//                perror("accept");
//                exit(EXIT_FAILURE);
//            }
//            fcntl(newSocket, F_SETFL, O_NONBLOCK);
//            cout << "New connection accepted" << endl;
//
//// Add the new socket to the list of sockets to monitor
//            fds[0].fd = newSocket;
//            fds[0].events = POLLIN;
//        }
//
//// Check for activity on the existing sockets
//        for (int i = 1; i < numSocketsReady; i++) {
//            if (fds[i].revents & POLLIN) {
//// Receive the data from the client
//                if (recv(fds[i].fd, buffer, BUFFER_SIZE, 0) <= 0) {
//// Error or connection closed by client
//                    close(fds[i].fd);
//                    fds[i].fd = -1;
//                } else {
//// Send the response and file
//                    sendFile(fds[i].fd, videoFilePath);
//                    close(fds[i].fd);
//                    fds[i].fd = -1;
//                }
//            }
//        }
//    }
//
//    return 0;
//}