#include "file_server.h"

int main() {
    FileServer server;
    server.start();

    std::cout << "Press enter to stop..." << std::endl;
    std::cin.get();

    server.stop();

    return 0;
}
