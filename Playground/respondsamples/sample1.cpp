void sendResponse(int clientSocket, const std::string& path) {
    // Check if the requested file exists and is readable
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == -1 || !S_ISREG(fileStat.st_mode) || !(fileStat.st_mode & S_IRUSR)) {
        // File not found or not readable
        std::string message = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(clientSocket, message.c_str(), message.length(), 0);
        return;
    }

    // Determine the file's MIME type based on the file extension
    std::string mimeType = getMimeType(path);

    // Send the content type header
    std::ostringstream headerStream;
    headerStream << "HTTP/1.1 200 OK\r\n";
    headerStream << "Content-Type: " << mimeType << "\r\n";

    // Check if the requested file is a media file
    if (isMediaFile(mimeType)) {
        // Get the requested range of bytes
        int startByte = 0;
        int endByte = fileStat.st_size - 1;
        bool rangeRequested = false;
        std::string rangeHeader;
        if (getRequestRange(path, &startByte, &endByte)) {
            rangeRequested = true;
            rangeHeader = "Content-Range: bytes " + std::to_string(startByte) + "-" + std::to_string(endByte) + "/" + std::to_string(fileStat.st_size) + "\r\n";
        }

        // Send the headers
        headerStream << "Accept-Ranges: bytes\r\n";
        if (rangeRequested) {
            headerStream << "Content-Length: " << (endByte - startByte + 1) << "\r\n";
            headerStream << rangeHeader;
            headerStream << "Content-Type: " << mimeType << "\r\n";
            headerStream << "Connection: Keep-Alive\r\n";
            headerStream << "\r\n";
        } else {
            headerStream << "Content-Length: " << fileStat.st_size << "\r\n";
            headerStream << "Content-Type: " << mimeType << "\r\n";
            headerStream << "Connection: Keep-Alive\r\n";
            headerStream << "\r\n";
        }

        // Send the headers to the client
        std::string header = headerStream.str();
        send(clientSocket, header.c_str(), header.length(), 0);

        // Send the requested range of bytes from the file
        off_t offset = startByte;
        size_t count = endByte - startByte + 1;
        sendfile(clientSocket, file.fd(), &offset, count);
    } else {
        // Send the content length header
        headerStream << "Content-Length: " << fileStat.st_size << "\r\n";
        headerStream << "Connection: Keep-Alive\r\n";
        headerStream << "\r\n";

        // Send the headers to the client
        std::string header = headerStream.str();
        send(clientSocket, header.c_str(), header.length(), 0);

        // Send the entire file to the client using sendfile
    // Open the file
    int fileDescriptor = open(path.c_str(), O_RDONLY);
    if (fileDescriptor == -1) {
        // File could not be opened
        std::string message = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
        send(clientSocket, message.c_str(), message.length(), 0);
        return;
    }

    // Send the file to the client using sendfile
    off_t offset = 0;
    size_t count = fileStat.st_size;
    sendfile(clientSocket, fileDescriptor, &offset, count);

    // Close the file descriptor
    close(fileDescriptor);
}


