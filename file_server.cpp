#include "file_server.h"

void FileServer::start() {

}

void FileServer::stop() {

}

void FileServer::run() {

}

void FileServer::startWritingFile(std::string name) {
    if (std::filesystem::exists(name)) {
        throw std::runtime_error("file is open");
    } else {
        filesLock.lock();
        files[name] = std::fstream(name, std::ios::app | std::ios::binary);
        filesLock.unlock();
    }
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
