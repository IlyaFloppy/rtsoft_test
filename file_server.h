#pragma once

#include <string>

class FileServer {
public:

    void start();

    void stop();

    void run();

private:

    void startWritingFile(std::string name);

    void writeBlock(std::string name, uint8_t *block, int size);

    void finishWritingFile(std::string name);

    const unsigned short port = 5678;
};
