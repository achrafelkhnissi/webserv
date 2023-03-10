#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <fstream>
#include <sstream>


using namespace std;

#define MAX_EVENTS 64
#define BUFFER_SIZE 1024

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

        // Convert the byte range strings to integers
        int startByte = std::stoi(startRange);
        int endByte = (endRange.empty() ? fileSize - 1 : std::stoi(endRange));

        // Calculate the content length and update the response header
        std::size_t contentLength = endByte - startByte + 1;
        responseHeader += "Content-Range: bytes " + std::to_string(startByte) + "-" + std::to_string(endByte) + "/" + std::to_string(fileSize) + "\r\n";
        responseHeader += "Content-Length: " + std::to_string(contentLength) + "\r\n";
        responseHeader += "Accept-Ranges: bytes\r\n";

        // Send the response header with a 206 Partial Content status code
        std::string partialContentHeader = "HTTP/1.1 206 Partial Content\r\n";
        partialContentHeader += responseHeader;
        send(clientSocket, partialContentHeader.c_str(), partialContentHeader.size(), 0);

        // Send the requested bytes from the file
        file.seekg(startByte, std::ios::beg);
        char* rangeBuffer = new char[contentLength];
        file.read(rangeBuffer, contentLength);
        send(clientSocket, rangeBuffer, contentLength, 0);
        delete[] rangeBuffer;
    } else {
        // Update the response header with the content length and send the response
        responseHeader += "Content-Length: " + std::to_string(fileSize) + "\r\n";
        send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);
        send(clientSocket, fileBuffer, fileSize, 0);
			}

	file.close();
	delete[] fileBuffer;
}

//void sendResponse(int socket, const char* filePath) {
//    std::ifstream fileStream(filePath, ios::in | ios::binary);
//
//    if (!fileStream) {
//        cerr << "Unable to open file" << endl;
//        return;
//    }
//
//    // Calculate the size of the file
//    fileStream.seekg(0, ios::end);
//    int fileSize = fileStream.tellg();
//    fileStream.seekg(0, ios::beg);
//
//    // Create the HTTP response header with the Content-Length field
//    ostringstream responseHeader;
////   stringstream responseHeader;
//    responseHeader << "HTTP/1.1 200 OK\r\n";
//	responseHeader << "Server: Webserv/1.0\r\n";
//    responseHeader << "Content-Type: video/mp4\r\n";
////	responseHeader << "Content-Type: image/jpeg\r\n";
//    responseHeader << "Content-Length: " << to_string(fileSize) << "\r\n";
//	responseHeader << "Connection: Keep-Alive\r\n";
//    responseHeader << "\r\n";
//
//    // Send the HTTP response header
//    const string& response = responseHeader.str();
//    send(socket, response.c_str(), response.size(), 0);
//
//    // Send the file data
//    char buffer[BUFFER_SIZE];
//    int bytesSent = 0;
//    while (fileStream) {
//        fileStream.read(buffer, BUFFER_SIZE);
//        int bytesRead = fileStream.gcount();
//        send(socket, buffer, bytesRead, 0);
//        bytesSent += bytesRead;
//    }
//
//    fileStream.close();
//}

int createSocket(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket.\n";
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind to port " << port << ".\n";
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen to port " << port << ".\n";
        exit(EXIT_FAILURE);
    }

    return serverSocket;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <port>\n";
		exit(EXIT_FAILURE);
	}

	int port = std::atoi(argv[1]);
	int serverSocket = createSocket(port);

	int flags = fcntl(serverSocket, F_GETFL, 0);
	if (flags < 0) {
		std::cerr << "Failed to get socket flags.\n";
		exit(EXIT_FAILURE);
	}
	if (fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
		std::cerr << "Failed to set socket as non-blocking.\n";
		exit(EXIT_FAILURE);
	}

	struct pollfd events[MAX_EVENTS];
	events[0].fd = serverSocket;
	events[0].events = POLLIN;

	std::vector<int> clients;
	char buffer[BUFFER_SIZE];

	while (true) {
		int n = poll(events, clients.size() + 1, -1);
		if (n < 0) {
			std::cerr << "Failed to poll for events.\n";
			exit(EXIT_FAILURE);
		}

		if (events[0].revents & POLLIN) {
			int clientSocket = accept(serverSocket, nullptr, nullptr);
			if (clientSocket < 0) {
				std::cerr << "Failed to accept incoming connection.\n";
			} else {
				int flags = fcntl(clientSocket, F_GETFL, 0);
				if (flags < 0) {
					std::cerr << "Failed to get socket flags.\n";
					exit(EXIT_FAILURE);
				}
				if (fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
					std::cerr << "Failed to set socket as non-blocking.\n";
					exit(EXIT_FAILURE);
				}

				clients.push_back(clientSocket);
				events[clients.size()].fd = clientSocket;
				events[clients.size()].events = POLLIN;
				std::cout << "New client connected.\n";
			}
		}

		for (int i = 1; i <= clients.size(); ++i) {
			if (events[i].revents & POLLIN) {
				int bytesRead = recv(clients[i - 1], buffer, BUFFER_SIZE, 0);
				if (bytesRead < 0) {
					if (errno != EWOULDBLOCK) {
						std::cerr << "Failed to receive data from client.\n";
						close(events[i].fd);
						clients.erase(clients.begin() + i - 1);
						events[i] = events[clients.size() + 1];
						--i;
					}
				} else if (bytesRead == 0) {
					std::cout << "Client disconnected.\n";
					close(events[i].fd);
					clients.erase(clients.begin() + i - 1);
					events[i] = events[clients.size() + 1];
					--i;
				} else {
					buffer[bytesRead] = '\0';
					std::cout << "Received data: " << buffer << "\n";
					//    const char* videoFilePath = "/Users/ael-khni/CLionProjects/webserv/www/media/image/myimage.jpg";
					const char* videoFilePath = "/Users/ael-khni/CLionProjects/webserv/www/media/video/video.mp4";
					sendResponse(events[i].fd, videoFilePath);
				}
			}
		}
	}

	close(serverSocket);
	return EXIT_SUCCESS;
}