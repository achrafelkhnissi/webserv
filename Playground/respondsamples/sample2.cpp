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
		responseHeader += "Content-Length: " + std::to_string(fileSize) + "\r\n\r\n";
		send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);
		sendfile(clientSocket, file.fd(), nullptr, fileSize);
	}

	delete[] fileBuffer;
	file.close();
}

