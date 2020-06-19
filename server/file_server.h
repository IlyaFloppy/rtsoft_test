#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <map>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

class FileServer {
public:

    void start();

    void stop();

    void run();

    void handle(int socket);

private:

    void startWritingFile(std::string name);

    void writeBlock(std::string name, uint8_t *block, int size);

    void finishWritingFile(std::string name);

    const unsigned short port = 5678;

    int serverDescriptor;
    sockaddr_in address;

    std::thread listener;
    std::map<int, std::thread> handlers;

    bool listening = true;
    std::map<std::string, std::fstream> files;
    std::mutex filesLock;
};
