#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1337
#define BUFFER_SIZE 1024

using namespace std;

void sendFile(int socket, const char* filePath) {
    ifstream fileStream(filePath, ios::in | ios::binary);

    if (!fileStream) {
        cerr << "Unable to open file" << endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fileStream) {
        fileStream.read(buffer, BUFFER_SIZE);
        int bytesRead = fileStream.gcount();
        send(socket, buffer, bytesRead, 0);
    }

    fileStream.close();
}

int main(int argc, char const *argv[]) {
    int serverSocket, newSocket, valRead;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char* videoFilePath = "/Users/fathjami/Desktop/webserv/www/media/video/video.mp4";
    char* response = "HTTP/1.1 200 OK\r\nContent-Type: video/mp4\r\n\r\n";

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

//        valRead = read(newSocket, buffer, BUFFER_SIZE);
        recv(newSocket, buffer, BUFFER_SIZE, 0);
        cout << buffer << endl;

        send(newSocket, response, strlen(response), 0);
        sendFile(newSocket, videoFilePath);

        close(newSocket);
    }

    return 0;
}
