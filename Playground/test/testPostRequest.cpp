#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>

void handle_post_request(int client_socket) {
    // read request headers from client
    std::string request_headers;
    char buffer[1024];
    int bytes_received;
    do {
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            // error reading request headers
            return;
        }
        request_headers.append(buffer, bytes_received);
    } while (bytes_received == sizeof(buffer));

    // parse request headers to get content length and content type
    std::istringstream request_stream(request_headers);
    std::string line;
    size_t content_length = 0;
    std::string content_type;
    while (std::getline(request_stream, line)) {
        if (line.find("Content-Length:") == 0) {
            content_length = std::stoi(line.substr(16));
        } else if (line.find("Content-Type:") == 0) {
            content_type = line.substr(14);
        }
    }
    // handle request based on content type
    if (content_type == "application/x-www-form-urlencoded") {
        // handle form data
        std::string form_data(&request_body[0], &request_body[0] + content_length);
        // process form data...
        // create response message
        std::stringstream response_stream;
        response_stream << "HTTP/1.1 200 OK\r\n\r\n";
        response_stream << "<html><body><h1>Form Submitted Successfully</h1></body></html>";
        std::string response = response_stream.str();
        send(client_socket, response.c_str(), response.length(), 0);
    } else if (content_type == "multipart/form-data") {
        // handle file upload
        std::string boundary = "--" + content_type.substr(content_type.find("boundary=") + 9);
        std::istringstream request_body_stream(std::string(&request_body[0], &request_body[0] + content_length));
        std::string line;
        std::getline(request_body_stream, line);
        while (std::getline(request_body_stream, line)) {
            if (line == boundary || line == boundary + "--") {
                // end of file upload
                break;
            } else if (line.find("Content-Disposition:") == 0) {
                std::string filename;
                size_t filename_start = line.find("filename=\"") + 10;
                size_t filename_end = line.find("\"", filename_start);
                if (filename_start != std::string::npos && filename_end != std::string::npos) {
                    filename = line.substr(filename_start, filename_end - filename_start);
                }
                std::getline(request_body_stream, line); // skip Content-Type line
                std::getline(request_body_stream, line); // skip empty line
                std::ofstream file_stream(_uploadPath + filename, std::ios::binary);
                if (file_stream.is_open()) {
                    while (std::getline(request_body_stream, line)) {
                        if (line == boundary || line == boundary + "--") {
                            // end of file upload
                            break;
                        }
                        file_stream << line << "\n";
                    }
                    file_stream.close();
                    // create response message
                    std::stringstream response_stream;
                    response_stream << "HTTP/1.1 200 OK\r\n\r\n";
                    response_stream << "<html><body><h1>File Uploaded Successfully</h1></body></html>";
                    std::string response = response_stream.str();
                    send(client_socket, response.c_str(), response.length(), 0);
                } else {
                    // error opening file stream
                    return;
                }
            }
        }
    } else {
        // unsupported content type
        std::stringstream response_stream;
        response_stream << "HTTP/1.1 400 Bad Request\r\n\r\n";
        response_stream << "<html><body><h1>Unsupported Content Type</h1></body></html>";
        std::string response = response_stream.str();
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

