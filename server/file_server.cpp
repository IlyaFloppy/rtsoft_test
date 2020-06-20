#include "file_server.h"

void FileServer::start() {
    serverDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverDescriptor == 0) {
        throw std::runtime_error("failed to create server socket");
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

    listener = std::thread([this]() { this->run(); });
}

void FileServer::stop() {
    listening = false;
    listener.join();
    for (auto &h : handlers) {
        h.second.join();
    }
}

void FileServer::run() {
    pollfd fds[1];
    fds[0].fd = serverDescriptor;
    fds[0].events = POLLIN;

    while (listening) {
        const int pollTimeoutMillis = 100;
        int pr = poll(fds, 1, pollTimeoutMillis);

        if (pr == -1) {
            std::cerr << "error occured while polling" << std::endl;
        } else if (pr == 0) {
            // nothing happened
        } else {
            if (fds[0].revents & POLLIN) {
                fds[0].revents = 0;

                int addressLength = sizeof(address);
                int socket = accept(serverDescriptor, (struct sockaddr *) &address, (socklen_t *) &addressLength);

                if (socket < 0) {
                    std::cerr << "failed to accept socket" << std::endl;
                }

                handlers[socket] = std::thread([this, socket]() { this->handle(socket); });
            }
        }
    }
}

void FileServer::handle(int socket) {
    int dataLength = -1;
    std::string filename;
    std::vector<uint8_t> buffer;
    while (dataLength != 0) {
        const int receiveBufferSize = 1 << 16;
        uint8_t receiveBuffer[receiveBufferSize] = {0};
        int readBytes = read(socket, receiveBuffer, receiveBufferSize);
        for (int i = 0; i < readBytes; ++i) {
            uint8_t byte = *(receiveBuffer + i);
            buffer.push_back(byte);

            if (byte == 0 && filename.length() == 0) {
                filename = std::string((char *) buffer.data());
                startWritingFile(filename);

                buffer.erase(buffer.begin(), buffer.begin() + filename.length() + 1);
            }
        }
        if (buffer.size() < 4) {
            continue;
        }

        dataLength =
                (((uint32_t) buffer[0]) << 24) +
                (((uint32_t) buffer[1]) << 16) +
                (((uint32_t) buffer[2]) << 8) +
                (((uint32_t) buffer[3]));

        if (buffer.size() < dataLength + 4) {
            continue;
        }

        writeBlock(filename, buffer.data() + 4, dataLength);
        buffer.erase(buffer.begin(), buffer.begin() + dataLength + 4);
    }
    finishWritingFile(filename);
}

void FileServer::startWritingFile(std::string name) {
    filesLock.lock();
    if (files.find(name) != files.end()) {
        throw std::runtime_error("file is open");
    } else {
        files[name] = std::fstream(name, std::ios::out | std::ios::binary);
    }
    filesLock.unlock();
}

void FileServer::writeBlock(std::string name, uint8_t *block, int size) {
    filesLock.lock();
    files[name].write((char *) block, size);
    filesLock.unlock();
}

void FileServer::finishWritingFile(std::string name) {
    filesLock.lock();
    files[name].close();
    files.erase(name);
    filesLock.unlock();
}
