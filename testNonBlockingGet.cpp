#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

using namespace std;

void send_video(int client_sock, const string& video_filename);

int main(int argc, char** argv) {

    // Parse the command line arguments
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " port_number" << endl;
        exit(1);
    }
    int port_number = atoi(argv[1]);

    // Create the server socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        cerr << "Error creating socket: " << strerror(errno) << endl;
        exit(1);
    }

    // Set the server socket to be non-blocking
    int flags = fcntl(server_sock, F_GETFL, 0);
    fcntl(server_sock, F_SETFL, flags | O_NONBLOCK);

    // Bind the socket to the local address and port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port_number);
    if (bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket: " << strerror(errno) << endl;
        exit(1);
    }

    // Start listening for incoming connections
    if (listen(server_sock, 10) < 0) {
        cerr << "Error listening on socket: " << strerror(errno) << endl;
        exit(1);
    }
    cout << "Server listening on port " << port_number << endl;

    // Accept incoming connections and handle them
    while (true) {
        // Wait for a connection
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*) &client_addr, &client_addr_len);

        // If there was an error accepting the connection, log an error and continue
        if (client_sock < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            continue;
        } else if (client_sock < 0) {
            cerr << "Error accepting connection: " << strerror(errno) << endl;
            continue;
        }

        // Handle the client request
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = recv(client_sock, buffer, sizeof(buffer), 0);

        // If there was an error reading from the client, log an error and close the connection
        if (bytes_read < 0) {
            cerr << "Error reading from client: " << strerror(errno) << endl;
            close(client_sock);
            continue;
        }

			const char* videoFilePath = "/Users/ael-khni/CLionProjects/webserv/www/media/video/video.mp4";

        // Send the video to the client
        send_video(client_sock, videoFilePath);

        // Close the client connection
        close(client_sock);
    }

    // Close the server socket
    close(server_sock);

    return 0;
}


void send_video(int client_sock, const string& video_filename) {

    // Open the video file
    ifstream video_file(video_filename, ios::binary);

    // Get the file size
    video_file.seekg(0, ios::end);
    int video_size = video_file.tellg();
    video_file.seekg(0, ios::beg);

    // Set the headers for the HTTP response
    ostringstream response_headers;
    response_headers << "HTTP/1.1 200 OK\r\n";
    response_headers << "Server: MyServer\r\n";
    response_headers << "Content-Type: video/mp4\r\n";
    response_headers << "Content-Length: " << video_size << "\r\n";
    response_headers << "Connection: Keep-Alive\r\n\r\n";

    // Send the headers to the client
    send(client_sock, response_headers.str().c_str(), response_headers.str().length(), 0);

    // Set up the poll structure
    struct pollfd pfd = {client_sock, POLLOUT | POLLWRNORM, 0};

    // Loop until the video has been fully sent
    char buffer[1024];
    int sent = 0;
    int bytes_read;

    while (sent < video_size) {
        // Wait for the socket to be writable
        poll(&pfd, 1, -1);

        // Read from the file
        video_file.read(buffer, sizeof(buffer));
        bytes_read = video_file.gcount();

        // Send the data to the client
        int bytes_sent = send(client_sock, buffer, bytes_read, 0);
        if (bytes_sent < 0) {
            cerr << "Error sending video: " << strerror(errno) << endl;
            break;
        }

        // Update the total number of bytes sent
        sent += bytes_sent;
    }

    // Close the file
    video_file.close();
}
