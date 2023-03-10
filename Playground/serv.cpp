#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>


#define PORT 4242
#define BUFFER_SIZE 4096

using namespace std;



void sendResponse(int clientSocket, const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return;
    }

    std::string responseHeader = "HTTP/1.1 200 OK\r\n";

    // Check if the requested file is a media file
    std::string fileExtension = filePath.substr(filePath.find_last_of(".") + 1);
    bool isMedia = (fileExtension == "mp3" || fileExtension == "mp4" || fileExtension == "avi");

    if (isMedia) {
        responseHeader += "Content-Type: video/mp4\r\n";
    } else {
        responseHeader += "Content-Type: text/html\r\n";
    }

    // Read the file contents into a buffer
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    char* fileBuffer = new char[fileSize];
    file.read(fileBuffer, fileSize);

    // Check if the client sent a Range request
    std::string rangeHeader = "Range: bytes=";
    char buffer[BUFFER_SIZE];
    int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, MSG_PEEK);
    buffer[bytesRead] = '\0';

    std::size_t rangePos = std::string(buffer).find(rangeHeader);
    if (rangePos != std::string::npos) {
        // Parse the byte range from the Range request
        std::string byteRange = std::string(buffer).substr(rangePos + rangeHeader.size());
        std::size_t delimiterPos = byteRange.find("-");
        std::string startRange = byteRange.substr(0, delimiterPos);
		std::string endRange = byteRange.substr(delimiterPos + 1);
//        std::string endbyteRange.substr(delimiterPos + 1);
    std::size_t startByte = std::stoi(startRange);
    std::size_t endByte = std::stoi(endRange);

    // Send a partial content response with a 206 status code
    responseHeader = "HTTP/1.1 206 Partial Content\r\n";

    // Update the Content-Range header
    std::string contentRangeHeader = "Content-Range: bytes " + startRange + "-" + endRange + "/" + std::to_string(fileSize) + "\r\n";
    responseHeader += contentRangeHeader;

    // Update the Content-Length header
    std::string contentLengthHeader = "Content-Length: " + std::to_string(endByte - startByte + 1) + "\r\n";
    responseHeader += contentLengthHeader;

    // Send the response header
    send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);

    // Seek to the requested byte range in the file
    file.seekg(startByte, std::ios::beg);

    // Send the requested bytes from the file to the client
    std::streamsize bytesToSend = endByte - startByte + 1;
    while (bytesToSend > 0) {
        std::streamsize chunkSize = std::min(bytesToSend, static_cast<std::streamsize>(BUFFER_SIZE));
        file.read(fileBuffer, chunkSize);
        send(clientSocket, fileBuffer, chunkSize, 0);
        bytesToSend -= chunkSize;
    }
} else {
    // Send the full file contents to the client
	int fd = open(filePath.c_str(), O_RDONLY);
    responseHeader += "Content-Length: " + std::to_string(fileSize) + "\r\n\r\n";
    send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);
//	sendfile(clientSocket, fd, NULL, fileSize, NULL, 0);
	char buffer_[BUFFER_SIZE];
    int bytesSent = 0;
    while (file) {
        file.read(buffer_, BUFFER_SIZE);
        int bytesR= file.gcount();
        send(clientSocket, buffer_, bytesR, 0);
    }

    file.close();
	close(fd);
}

delete[] fileBuffer;
file.close();
}

int main() {
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
if (serverSocket == -1) {
    std::cerr << "Failed to create socket.\n";
    return 1;
}

// Set the socket as non-blocking
int flags = fcntl(serverSocket, F_GETFL, 0);
fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);

int bindResult = bind(serverSocket, (struct sockaddr*)&address, sizeof(address));
if (bindResult == -1) {
    std::cerr << "Failed to bind socket.\n";
    return 1;
}

int listenResult = listen(serverSocket, SOMAXCONN);
if (listenResult == -1) {
    std::cerr << "Failed to listen on socket.\n";
    return 1;
}

struct pollfd pollArray[1];
pollArray[0].fd = serverSocket;
pollArray[0].events = POLLIN;

while (true) {
    int pollResult = poll(pollArray, 1, -1);

    if (pollResult == -1) {
        std::cerr << "Failed to poll sockets.\n";
        return 1;
    }

    if (pollArray[0].revents & POLLIN) {
        // New connection
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

        if (clientSocket == -1) {
			// clientSocket == -1 means that there are no pending connections, so we simply continue polling
if (errno == EAGAIN || errno == EWOULDBLOCK) {
continue;
} else {
std::cerr << "Failed to accept new connection.\n";
continue;
}

		}
		        // Set the client socket as non-blocking
        int clientFlags = fcntl(clientSocket, F_GETFL, 0);
        fcntl(clientSocket, F_SETFL, clientFlags | O_NONBLOCK);

        // Add the client socket to the poll array
        pollArray[1].fd = clientSocket;
        pollArray[1].events = POLLIN;
    }

    if (pollArray[1].revents & POLLIN) {
        // Client has sent a request
        char requestBuffer[BUFFER_SIZE];
        int bytesRead = recv(pollArray[1].fd, requestBuffer, BUFFER_SIZE, 0);

        if (bytesRead == -1) {
            // Error occurred while reading from the client socket
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                std::cerr << "Error occurred while reading from client socket.\n";
                close(pollArray[1].fd);
                pollArray[1].fd = -1;
                continue;
            }
        }

        if (bytesRead == 0) {
            // Client has disconnected
            close(pollArray[1].fd);
            pollArray[1].fd = -1;
            continue;
        }

        std::string request(requestBuffer, bytesRead);

        // Parse the HTTP request and send the response
        std::string path = "/Users/ael-khni/CLionProjects/webserv/www/media/video/video.mp4";
        sendResponse(pollArray[1].fd, path);
        close(pollArray[1].fd);
        pollArray[1].fd = -1;
    }
}

close(serverSocket);
return 0;
}

