#include "file_server.h"

void FileServer::start() {
    serverDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverDescriptor == 0) {
        throw std::runtime_error("failed to create server socket");
    }

    int opt = 1;
    if (setsockopt(serverDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        throw std::runtime_error("failed to set socket opt");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverDescriptor, (sockaddr *) &address, sizeof(address)) < 0) {
        throw std::runtime_error("failed to bind to port");
    }

    if (listen(serverDescriptor, 8) < 0) {
        throw std::runtime_error("failed to start listening");
    }

    std::thread listener([this]() { this->run(); });
}

void FileServer::stop() {

}

void FileServer::run() {
    while (listening) {
        int addressLength = sizeof(address);
        int socket = accept(serverDescriptor, (struct sockaddr *) &address, (socklen_t *) &addressLength);

        if (socket < 0) {
            std::cerr << "failed to accept socket" << std::endl;
        }

        std::thread handler([this, socket]() { this->handle(socket); });
    }
}

void FileServer::handle(int socket) {
    int dataLength = -1;
    std::string filename;
    while (dataLength != 0) {
        const int receiveBufferSize = 1 << 12;
        uint8_t receiveBuffer[receiveBufferSize] = {0};
        int readBytes = read(socket, receiveBuffer, receiveBufferSize);
        dataLength =
                (receiveBuffer[0] << 24) +
                (receiveBuffer[1] << 16) +
                (receiveBuffer[2] << 8) +
                (receiveBuffer[3]);

        bool isFirstBlock = dataLength < 0;
        if (isFirstBlock) {
            filename = std::string((char *) (receiveBuffer + 4));
        }
        startWritingFile(filename);

        int offset = 4;
        if (isFirstBlock) {
            offset += filename.length();
        }
        writeBlock(filename, receiveBuffer + offset, dataLength);
    }
    finishWritingFile(filename);
}

void FileServer::startWritingFile(std::string name) {

}

void FileServer::writeBlock(std::string name, uint8_t *block, int size) {

}

void FileServer::finishWritingFile(std::string name) {

}
